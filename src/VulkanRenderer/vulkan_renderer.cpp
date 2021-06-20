#include "vulkan_renderer.hpp"

namespace slash {

VulkanRenderer::VulkanRenderer(Window *window) {
  vcore_ = std::make_unique<VulkanCore>(window->get_native_window());
  auto queue_families = vcore_->get_queue_families();
  graphics_queue_ = std::make_unique<VulkanQueue>(vcore_.get(), queue_families.graphicsFamily.value());
  present_queue_ = std::make_unique<VulkanQueue>(vcore_.get(), queue_families.presentFamily.value());
  swap_chain_ =
      std::make_unique<VulkanSwapChain>(vcore_.get(), std::vector{graphics_queue_.get(), present_queue_.get()});
  render_pass_ = std::make_unique<VulkanRenderPass>(vcore_.get(), swap_chain_.get());

  descriptor_set_layout_ = std::make_unique<VulkanDescriptorSetLayout>(vcore_.get());
  descriptor_set_layout_->AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     VK_SHADER_STAGE_VERTEX_BIT);
  descriptor_set_layout_->AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     VK_SHADER_STAGE_VERTEX_BIT);
  descriptor_set_layout_->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                     VK_SHADER_STAGE_FRAGMENT_BIT);
  descriptor_set_layout_->CreateLayout();

  vertex_shader_ = std::make_unique<VulkanShader>(
      vcore_.get(), "src/Shaders/vert.spv");
  fragment_shader_ = std::make_unique<VulkanShader>(
      vcore_.get(), "src/Shaders/frag.spv");
  pipeline_ =
      std::make_unique<VulkanPipeline>(vcore_.get(), render_pass_.get(), swap_chain_.get(), vertex_shader_.get(),
                                       fragment_shader_.get(), descriptor_set_layout_.get());

  depth_ = std::make_unique<VulkanTexture>(
      vcore_.get(), swap_chain_->GetExtent().width, swap_chain_->GetExtent().height,
      vcore_->find_supported_format(
          {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
           VK_FORMAT_D32_SFLOAT_S8_UINT},
          VK_IMAGE_TILING_OPTIMAL,
          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  for (auto &depth = depth_;
      auto &swap_chain_image : swap_chain_->GetImageViews()) {
    frame_buffers_.emplace_back(std::make_unique<VulkanFrameBuffer>(
        vcore_.get(), render_pass_.get(), swap_chain_->GetExtent().width,
        swap_chain_->GetExtent().height,
        std::vector{swap_chain_image, depth->GetImageView()}));
  }

  descriptor_manager_ = std::make_unique<VulkanDescriptorManager>(vcore_.get());
  command_buffer_manager_ = std::make_unique<VulkanCommandBufferManager>(
      vcore_.get(), queue_families.graphicsFamily.value());
  //

  inflight_fences_.emplace_back(std::make_unique<VulkanFence>(vcore_.get()));
  inflight_fences_.emplace_back(std::make_unique<VulkanFence>(vcore_.get()));
  image_available_semaphores_.emplace_back(std::make_unique<VulkanSemaphore>(vcore_.get()));
  image_available_semaphores_.emplace_back(std::make_unique<VulkanSemaphore>(vcore_.get()));
  render_finished_semaphores_.emplace_back(std::make_unique<VulkanSemaphore>(vcore_.get()));
  render_finished_semaphores_.emplace_back(std::make_unique<VulkanSemaphore>(vcore_.get()));
}

VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(vcore_->get_device());
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
  frame_buffers_.clear();
  depth_.reset();
  pipeline_.reset();
  render_pass_.reset();
  swap_chain_.reset();

  swap_chain_ =
      std::make_unique<VulkanSwapChain>(vcore_.get(), std::vector{graphics_queue_.get(), present_queue_.get()});
  render_pass_ = std::make_unique<VulkanRenderPass>(vcore_.get(), swap_chain_.get());
  pipeline_ =
      std::make_unique<VulkanPipeline>(vcore_.get(), render_pass_.get(), swap_chain_.get(), vertex_shader_.get(),
                                       fragment_shader_.get(), descriptor_set_layout_.get());

  depth_ = std::make_unique<VulkanTexture>(
      vcore_.get(), swap_chain_->GetExtent().width, swap_chain_->GetExtent().height,
      vcore_->find_supported_format(
          {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
           VK_FORMAT_D32_SFLOAT_S8_UINT},
          VK_IMAGE_TILING_OPTIMAL,
          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  for (auto &depth = depth_;
      auto &swap_chain_image : swap_chain_->GetImageViews()) {
    frame_buffers_.emplace_back(std::make_unique<VulkanFrameBuffer>(
        vcore_.get(), render_pass_.get(), swap_chain_->GetExtent().width,
        swap_chain_->GetExtent().height,
        std::vector{depth->GetImageView(), swap_chain_image}));
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
  return descriptor_manager_->CreateDescriptorSet(descriptor_set_layout_.get());
}

void VulkanRenderer::new_frame() { command_buffer_manager_->SwitchFrame(); }

std::shared_ptr<VulkanCommandBuffer>
VulkanRenderer::start_render_command() const {
  auto command_buffer = command_buffer_manager_->CreateFrameCommandBuffer();
  vkResetCommandBuffer(command_buffer->GetBuffer(), 0);
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