#include "vulkan_imgui.hpp"
#include "Core/log.hpp"

namespace slash {

void VulkanImGui::Init(GLFWwindow *window, VkInstance instance,
                       VkPhysicalDevice physical_device, VkDevice device,
                       uint32_t queue_family, VkQueue graphics_queue,
                       VkPipelineCache pipeline_cache,
                       VkFormat render_pass_color_format,
                       VkFormat render_pass_depth_format,
                       uint32_t image_count) {
  window_ = window;
  instance_ = instance;
  physical_device_ = physical_device;
  device_ = device;
  queue_family_ = queue_family;
  graphics_queue_ = graphics_queue;
  pipeline_cache_ = pipeline_cache;
  render_pass_color_format_ = render_pass_color_format;
  render_pass_depth_format_ = render_pass_depth_format;
  image_count_ = image_count;
  SL_CORE_INFO("INITIALIZING IMGUI");
//  CreateDescriptorPool();
//  CreateCommandPool();
//  CreateCommandBuffers();
//  CreateRenderPass();
//
//  IMGUI_CHECKVERSION();
//  ImGui::CreateContext();
//  //  ImGuiIO &io = ImGui::GetIO();
//  //  (void)io;
//  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
//  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
//  // Enable Gamepad Controls
//
//  ImGui::StyleColorsDark();
//
//  ImGui_ImplGlfw_InitForVulkan(window_, false);
//  ImGui_ImplVulkan_InitInfo init_info = {};
//  init_info.Instance = instance_;
//  init_info.PhysicalDevice = physical_device_;
//  init_info.Device = device_;
//  init_info.QueueFamily = queue_family_;
//  init_info.Queue = graphics_queue_;
//  init_info.PipelineCache = pipeline_cache_;
//  init_info.DescriptorPool = descriptor_pool_;
//  init_info.Allocator = nullptr;
//  init_info.MinImageCount = 2;
//  init_info.ImageCount = image_count_;
//  init_info.CheckVkResultFn = nullptr;
//  ImGui_ImplVulkan_Init(&init_info, render_pass_);
//
//  VkCommandBufferAllocateInfo alloc_info = {};
//  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//  alloc_info.commandPool = command_pool_;
//  alloc_info.commandBufferCount = 1;
//  VkCommandBuffer command_buffer;
//  vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);
//  VkCommandBufferBeginInfo begin_info = {};
//  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//  vkBeginCommandBuffer(command_buffer, &begin_info);
//  ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
//  vkEndCommandBuffer(command_buffer);
//
//  VkSubmitInfo submit_info = {};
//  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//  submit_info.commandBufferCount = 1;
//  submit_info.pCommandBuffers = &command_buffer;
//  vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
//  vkQueueWaitIdle(graphics_queue_);
//  vkFreeCommandBuffers(device_, command_pool_, 1, &command_buffer);
//
//  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void VulkanImGui::Destroy() {
  SL_CORE_INFO("DESTROYING IMGUI");
//
//  vkDestroyCommandPool(device_, command_pool_, nullptr);
//  vkDestroyRenderPass(device_, render_pass_, nullptr);
//  vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
//
//  ImGui_ImplVulkan_Shutdown();
//  ImGui_ImplGlfw_Shutdown();
//  ImGui::DestroyContext();
}

void VulkanImGui::SetDrawFn(ImGuiDrawFn& fn) {
 draw_fn_ = fn;
}

void VulkanImGui::UpdateCommandBuffers(VkFramebuffer framebuffer,
                                       VkExtent2D extend) {

//  static float f = 0.0f;
//  static int counter = 0;
//  static bool show_demo_window = true;
//  ImGui_ImplVulkan_NewFrame();
//  ImGui_ImplGlfw_NewFrame();
//  ImGui::NewFrame();
//  if (draw_fn_) draw_fn_();
//  ImGui::ShowDemoWindow(&show_demo_window);
////  ImGui::Begin("Hello, world!");
////  ImGui::Text("This is some useful text.");
////  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
////  if (ImGui::Button("Button"))
////    counter++;
////  ImGui::SameLine();
////  ImGui::Text("counter = %d", counter);
////  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
////              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
////  ImGui::End();
//  ImGui::Render();
//
//  vkDeviceWaitIdle(device_);
//  vkResetCommandBuffer(command_buffer_, 0);
//  VkCommandBufferBeginInfo begin_info = {};
//  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//  begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
//  begin_info.pInheritanceInfo = nullptr; //&inheritance_info;
//  if (vkBeginCommandBuffer(command_buffer_, &begin_info) != VK_SUCCESS) {
//    throw std::runtime_error("failed to begin recording command buffer");
//  }
//  VkRenderPassBeginInfo render_pass_info = {};
//  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//  render_pass_info.renderPass = render_pass_;
//  render_pass_info.framebuffer = framebuffer;
//  render_pass_info.renderArea.offset = {0, 0};
//  render_pass_info.renderArea.extent = extend;
//  std::array<VkClearValue, 2> clear_values = {};
//  clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
//  clear_values[1].depthStencil = {1.0f, 0};
//  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
//  render_pass_info.pClearValues = clear_values.data();
//  vkCmdBeginRenderPass(command_buffer_, &render_pass_info,
//                       VK_SUBPASS_CONTENTS_INLINE);
//  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer_);
//  vkCmdEndRenderPass(command_buffer_);
//  if (vkEndCommandBuffer(command_buffer_) != VK_SUCCESS) {
//    throw std::runtime_error("failed to record imgui command buffer");
//  }
}

VkCommandBuffer VulkanImGui::CommandBuffer() {
  return command_buffer_;
}

void VulkanImGui::SetMinimalImageCount(uint32_t image_count) {
//  ImGui_ImplVulkan_SetMinImageCount(image_count);
}

void VulkanImGui::CreateDescriptorPool() {
  std::array<VkDescriptorPoolSize, 11> pool_sizes = {
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = 1000;
  if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &descriptor_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor_pool_");
  }
}

void VulkanImGui::CreateCommandPool() {
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool");
  }
}
void VulkanImGui::CreateCommandBuffers() {
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level =
      VK_COMMAND_BUFFER_LEVEL_PRIMARY; // VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  alloc_info.commandBufferCount = 1;
  if (vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command buffers");
  }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = 0;
  begin_info.pInheritanceInfo = nullptr;
  if (vkBeginCommandBuffer(command_buffer_, &begin_info) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer");
  }
  if (vkEndCommandBuffer(command_buffer_) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}
void VulkanImGui::CreateRenderPass() {
  VkAttachmentDescription color_attachment = {};
  color_attachment.format = render_pass_color_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.finalLayout =
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //
                                       // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth_attachment = {};
  depth_attachment.format = render_pass_depth_format_;
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  std::array<VkSubpassDescription, 1> subpasses = {};
  subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpasses[0].colorAttachmentCount = 1;
  subpasses[0].pColorAttachments = &color_attachment_ref;
  subpasses[0].pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachemnts = {color_attachment,
                                                        depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachemnts.size());
  render_pass_info.pAttachments = attachemnts.data();
  render_pass_info.subpassCount = static_cast<uint32_t>(subpasses.size());
  render_pass_info.pSubpasses = subpasses.data();
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create imgui render pass");
  }
}

} // namespace slash
