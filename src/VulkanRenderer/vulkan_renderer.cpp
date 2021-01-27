#include "vulkan_renderer.hpp"

namespace slash {

VulkanRenderer::VulkanRenderer(Window *window) {
  vcore_ = new VulkanCore(window->get_native_window());
  auto queue_families = vcore_->get_queue_families();
  graphics_queue_ = new VulkanQueue(vcore_, queue_families.graphicsFamily.value());
  present_queue_ = new VulkanQueue(vcore_, queue_families.presentFamily.value());
  swap_chain_ = new VulkanSwapChain(vcore_, {graphics_queue_, present_queue_});
  render_pass_ = new VulkanRenderPass(vcore_, swap_chain_);

  descriptor_set_layout_ = new VulkanDescriptorSetLayout(vcore_);
  descriptor_set_layout_->AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     VK_SHADER_STAGE_VERTEX_BIT);
  descriptor_set_layout_->AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     VK_SHADER_STAGE_VERTEX_BIT);
  descriptor_set_layout_->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                     VK_SHADER_STAGE_FRAGMENT_BIT);
  descriptor_set_layout_->CreateLayout();

  VulkanShader vertex_shader(
      vcore_, "/home/antaraz/Projects/SlashEngine/src/Shaders/vert.spv");
  VulkanShader fragment_shader(
      vcore_, "/home/antaraz/Projects/SlashEngine/src/Shaders/frag.spv");
  pipeline_ = new VulkanPipeline(vcore_, render_pass_, swap_chain_, vertex_shader,
                                 fragment_shader, descriptor_set_layout_);

  depth_ = new VulkanTexture(
      vcore_, swap_chain_->GetExtent().width, swap_chain_->GetExtent().height,
      vcore_->find_supported_format(
          {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
           VK_FORMAT_D32_SFLOAT_S8_UINT},
          VK_IMAGE_TILING_OPTIMAL,
          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  for (auto &depth = depth_;
      auto &swap_chain_image : swap_chain_->GetImageViews()) {
    frame_buffers_.emplace_back(new VulkanFrameBuffer(
        vcore_, render_pass_, swap_chain_->GetExtent().width,
        swap_chain_->GetExtent().height,
        {swap_chain_image, depth->GetImageView()}));
  }

  descriptor_manager_ = std::make_unique<VulkanDescriptorManager>(vcore_);
  command_buffer_manager_ = std::make_unique<VulkanCommandBufferManager>(
      vcore_, queue_families.graphicsFamily.value());
  //

  inflight_fences_.emplace_back(new VulkanFence(vcore_));
  inflight_fences_.emplace_back(new VulkanFence(vcore_));
  image_available_semaphores_.emplace_back(new VulkanSemaphore(vcore_));
  image_available_semaphores_.emplace_back(new VulkanSemaphore(vcore_));
  render_finished_semaphores_.emplace_back(new VulkanSemaphore(vcore_));
  render_finished_semaphores_.emplace_back(new VulkanSemaphore(vcore_));
}

VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(vcore_->get_device());
  delete inflight_fences_[0];
  delete inflight_fences_[1];
  delete image_available_semaphores_[0];
  delete image_available_semaphores_[1];
  delete render_finished_semaphores_[0];
  delete render_finished_semaphores_[1];

  command_buffer_manager_.reset();
  descriptor_manager_.reset();

  delete depth_;
  for (auto &frame_buffer : frame_buffers_)
    delete frame_buffer;

  delete pipeline_;
  delete descriptor_set_layout_;
  delete render_pass_;
  delete swap_chain_;
  delete present_queue_;
  delete graphics_queue_;
  delete vcore_;
}

void VulkanRenderer::draw_frame(double time) {
  vkWaitForFences(vcore_->get_device(), 1,
                  &inflight_fences_[current_frame_]->GetFence(), VK_TRUE,
                  UINT64_MAX);
  auto result = vkAcquireNextImageKHR(
      vcore_->get_device(), swap_chain_->GetSwapChain(), UINT64_MAX,
      image_available_semaphores_[current_frame_]->GetSemaphore(),
      VK_NULL_HANDLE, &current_index_);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreate_swap_chain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {
      image_available_semaphores_[current_frame_]->GetSemaphore()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  auto command_buffers = command_buffer_manager_->GetBuffers();
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = waitSemaphores;
  submit_info.pWaitDstStageMask = waitStages;
  submit_info.commandBufferCount =
      static_cast<uint32_t>(command_buffers.size());
  submit_info.pCommandBuffers = command_buffers.data();
  VkSemaphore signalSemaphores[] = {
      render_finished_semaphores_[current_frame_]->GetSemaphore()};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signalSemaphores;
  vkResetFences(vcore_->get_device(), 1,
                &inflight_fences_[current_frame_]->GetFence());
  if (vkQueueSubmit(graphics_queue_->GetQueue(), 1, &submit_info,
                    inflight_fences_[current_frame_]->GetFence()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer");
  }
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {swap_chain_->GetSwapChain()};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapChains;
  present_info.pImageIndices = &current_index_;
  present_info.pResults = nullptr;
  result = vkQueuePresentKHR(present_queue_->GetQueue(), &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    //    window_data_->resized = false;
    recreate_swap_chain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image");
  }
  current_frame_ = (current_frame_ + 1) % max_frames_;
}

void VulkanRenderer::wait_idle() {
  vkDeviceWaitIdle(vcore_->get_device());
}

void VulkanRenderer::recreate_swap_chain() {
  vkDeviceWaitIdle(vcore_->get_device());
  for (auto &frame_buffer : frame_buffers_) {
    delete frame_buffer;
  }
  delete depth_;

  delete pipeline_;
  delete render_pass_;
  delete swap_chain_;

  swap_chain_ = new VulkanSwapChain(vcore_, {graphics_queue_, present_queue_});
  // TODO wait for fix
  render_pass_ = new VulkanRenderPass(vcore_, swap_chain_);
  VulkanShader vertex_shader(
      vcore_, "/home/antaraz/Projects/SlashEngine/src/Shaders/vert.spv");
  VulkanShader fragment_shader(
      vcore_, "/home/antaraz/Projects/SlashEngine/src/Shaders/frag.spv");
  pipeline_ =
      new VulkanPipeline(vcore_, render_pass_, swap_chain_, vertex_shader,
                         fragment_shader, descriptor_set_layout_);

  depth_ = new VulkanTexture(
      vcore_, swap_chain_->GetExtent().width, swap_chain_->GetExtent().height,
      vcore_->find_supported_format(
          {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
           VK_FORMAT_D32_SFLOAT_S8_UINT},
          VK_IMAGE_TILING_OPTIMAL,
          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  for (auto &depth = depth_;
      auto &swap_chain_image : swap_chain_->GetImageViews()) {
    frame_buffers_.emplace_back(new VulkanFrameBuffer(
        vcore_, render_pass_, swap_chain_->GetExtent().width,
        swap_chain_->GetExtent().height,
        {depth->GetImageView(), swap_chain_image}));
  }
}

std::shared_ptr<VulkanCommandBuffer>
VulkanRenderer::begin_one_time_command() const {
  auto command = command_buffer_manager_->CreateBuffer();

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command->GetBuffer(), &begin_info);
  return command;
}

void VulkanRenderer::end_one_time_command(
    std::shared_ptr<VulkanCommandBuffer> command_buffer) {
  vkEndCommandBuffer(command_buffer->GetBuffer());
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  auto buffer = command_buffer->GetBuffer();
  submit_info.pCommandBuffers = &buffer;
  vkQueueSubmit(graphics_queue_->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue_->GetQueue());
  command_buffer_manager_->FreeBuffer(command_buffer);
}

VulkanDescriptorSet *VulkanRenderer::create_descriptor_set() const {
  return descriptor_manager_->CreateDescriptorSet(descriptor_set_layout_);
}

void VulkanRenderer::new_frame() { command_buffer_manager_->SwitchFrame(); }

std::shared_ptr<VulkanCommandBuffer>
VulkanRenderer::start_render_command() const {
  auto command_buffer = command_buffer_manager_->CreateFrameCommandBuffer();
  //  vkResetCommandBuffer(command_buffer->GetBuffer(), 0);
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = 0;
  begin_info.pInheritanceInfo = nullptr;
  if (vkBeginCommandBuffer(command_buffer->GetBuffer(), &begin_info) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer");
  }
  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass_->GetRenderPass();
  render_pass_info.framebuffer =
      frame_buffers_[(current_index_ + 1) % swap_chain_->GetImageViews().size()]->GetFrameBuffer();
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swap_chain_->GetExtent();
  std::array<VkClearValue, 2> clear_values = {};
  clear_values[0].color = {{0.25f, 0.25f, 0.25f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();
  vkCmdBeginRenderPass(command_buffer->GetBuffer(), &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(command_buffer->GetBuffer(),
                    VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetPipeline());
  return command_buffer;
}
void VulkanRenderer::end_render_command(
    std::shared_ptr<VulkanCommandBuffer> buffer) {
  vkCmdEndRenderPass(buffer->GetBuffer());
  if (vkEndCommandBuffer(buffer->GetBuffer()) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}

} // namespace slash