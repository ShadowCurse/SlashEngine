#include "vulkan_rendererAPI.hpp"
#include "Core/application.hpp"
#include "Core/log.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace slash {

void VulkanRendererAPI::AddWindow(std::shared_ptr<Window> window) {
  window_ = window;
}

void VulkanRendererAPI::CreateSubSystems() {
  buffer_manager_ = CreateUnique<VulkanBufferManager>(this);
}

void VulkanRendererAPI::DestroySubSystems() { buffer_manager_.reset(); }

void VulkanRendererAPI::Init() {
  CreateSubSystems();
  CreateInstance();
  SetUpDebugMessenger();
  GetWindowData();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateDescriptorSetLayout();
  CreateGraphicsPipeline();
  CreateCommandPool();
  CreateDepthResources();
  CreateFramebuffers();
  CreateTextureImage();
  CreateTextureImageView();
  CreateTextureSampler();
  //  CreateVertexBuffer();
  //  CreateIndexBuffer();

  //  BindVertexBuffer(global_vertices);
  //  BindIndexBuffer(global_indices);

  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateDescriptorSets();
  CreateCommandBuffers();
  //  UpdateCommandBuffers();
  CreateSemaphores();
  CreateFences();
}

void VulkanRendererAPI::Destroy() {
  vkDeviceWaitIdle(device_);
  for (size_t i(0); i < config_.k_max_frames_in_flight; ++i) {
    vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
    vkDestroyFence(device_, inflight_fences_[i], nullptr);
  }
  DestroySwapChain();
  vkDestroySampler(device_, texture_sampler_, nullptr);
  vkDestroyImageView(device_, texture_image_view_, nullptr);
  vkDestroyImage(device_, texture_image_, nullptr);
  vkFreeMemory(device_, texture_image_memory_, nullptr);
  vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
  //  vkDestroyBuffer(device_, vertex_buffer_, nullptr);
  //  vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
  //  vkDestroyBuffer(device_, index_buffer_, nullptr);
  //  vkFreeMemory(device_, index_buffer_memory_, nullptr);
  DestroySubSystems();
  vkDestroyCommandPool(device_, command_pool_, nullptr);
  vkDestroyDevice(device_, nullptr);
  if (config_.enable_validation_layers)
    DestroyDebugUtilsMessengerEXT(instance_, debug_utils_messenger_ext_,
                                  nullptr);
  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

void VulkanRendererAPI::DrawFrame(float time) {
  vkWaitForFences(device_, 1, &inflight_fences_[current_frame], VK_TRUE,
                  UINT64_MAX);
  uint32_t image_index;
  auto result = vkAcquireNextImageKHR(
      device_, swap_chain_, UINT64_MAX,
      image_available_semaphores_[current_frame], VK_NULL_HANDLE, &image_index);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    window_data_->resized = false;
    RecreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }
  if (images_inflight_[image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(device_, 1, &images_inflight_[image_index], VK_TRUE,
                    UINT64_MAX);
  }
  // imagesInFlight[imageIndex] = inFlightFences[current_frame];
  UpdateUniformBuffer(time, image_index);
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {image_available_semaphores_[current_frame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = waitSemaphores;
  submit_info.pWaitDstStageMask = waitStages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffers_[image_index];
  VkSemaphore signalSemaphores[] = {render_finished_semaphores_[current_frame]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signalSemaphores;
  vkResetFences(device_, 1, &inflight_fences_[current_frame]);
  if (vkQueueSubmit(graphics_queue_, 1, &submit_info,
                    inflight_fences_[current_frame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer");
  }
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {swap_chain_};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapChains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = nullptr;
  result = vkQueuePresentKHR(present_queue_, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window_data_->resized) {
    window_data_->resized = false;
    RecreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image");
  }
  current_frame = (current_frame + 1) % config_.k_max_frames_in_flight;
}

void VulkanRendererAPI::UpdateScene() { UpdateCommandBuffers(); }

void VulkanRendererAPI::BindVertexBuffer(size_t uid,
                                         const std::vector<Vertex> &vertices) {
  SL_CORE_INFO("Binding vertex buffer");
  buffer_manager_->AddVertexBuffer(uid, vertices);
}

void VulkanRendererAPI::BindIndexBuffer(size_t uid,
                                        const std::vector<uint16_t> &indices) {
  SL_CORE_INFO("Binding index buffer");
  buffer_manager_->AddIndexBuffer(uid, indices);
}

void VulkanRendererAPI::UnBindVertexBuffer(size_t uid) {
  buffer_manager_->RemoveVertexBuffer(uid);
}

void VulkanRendererAPI::UnBindIndexBuffer(size_t uid) {
  buffer_manager_->RemoveIndexBuffer(uid);
}

//////////////////////////////////////////////////////
/////////// PRIVATE FUNC /////////////////////////////
//////////////////////////////////////////////////////

void VulkanRendererAPI::CreateInstance() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = config_.instance_config.application_name.c_str();
  app_info.applicationVersion = config_.instance_config.application_version;
  app_info.pEngineName = config_.instance_config.engineName.c_str();
  app_info.engineVersion = config_.instance_config.engine_version;
  app_info.apiVersion = VK_API_VERSION_1_1;

  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensionsProps(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensionsProps.data());

  //  SL_CORE_INFO("Available extentions: ");
  //  for (const auto &ext : extensionsProps) {
  //    SL_CORE_INFO(ext.extensionName);
  //  }

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  auto extensions = GetRequaredExtentions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (config_.enable_validation_layers && !CheckValidationLayers()) {
    throw std::runtime_error("validation layers requested? but not available");
  }
  if (config_.enable_validation_layers && CheckValidationLayers()) {
    create_info.enabledLayerCount =
        static_cast<uint32_t>(config_.validation_layers.size());
    create_info.ppEnabledLayerNames = config_.validation_layers.data();
    SetUpDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = &debug_create_info;
  } else {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan instance");
  }
}

void VulkanRendererAPI::SetUpDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT debug_info;
  SetUpDebugMessengerCreateInfo(debug_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr,
                                   &debug_utils_messenger_ext_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan Debug Messenger");
  }
}

void VulkanRendererAPI::GetWindowData() {
  if (!window_)
    throw std::runtime_error("no windows assigned to renderer");
  window_data_ = static_cast<Window::WindowData *>(glfwGetWindowUserPointer(
      static_cast<GLFWwindow *>(window_->GetNativeWindow())));
}

void VulkanRendererAPI::CreateSurface() {
  if (glfwCreateWindowSurface(
          instance_,
          static_cast<GLFWwindow *>(
              slash::Application::Get().GetWindow().GetNativeWindow()),
          nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create widow surface");
  }
}

void VulkanRendererAPI::PickPhysicalDevice() {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  if (device_count == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (const auto &device : devices) {
    // TODO make some GPU score to pick one with most performance
    if (IsDeviceSuitable(device)) {
      physical_device_ = device;
      break;
    }
  }
  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a sutable GPU");
  }
  queue_family_indices_ = FindQueueFamilies(physical_device_);
}

void VulkanRendererAPI::CreateLogicalDevice() {
  std::vector<VkDeviceQueueCreateInfo> queue_createinfos;
  std::set<uint32_t> unique_queue_families = {
      queue_family_indices_.graphicsFamily.value(),
      queue_family_indices_.presentFamily.value()};

  float queue_priority = 1.0f; // must be between 0 and 1
  for ([[maybe_unused]] uint32_t queueFamily : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex =
        queue_family_indices_.graphicsFamily.value();
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_createinfos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features = {}; // features that will be in use
  device_features.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queue_createinfos.size());
  create_info.pQueueCreateInfos = queue_createinfos.data();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount =
      static_cast<uint32_t>(config_.device_extentions.size());
  create_info.ppEnabledExtensionNames = config_.device_extentions.data();

  if (config_.enable_validation_layers) {
    create_info.enabledLayerCount =
        static_cast<uint32_t>(config_.validation_layers.size());
    create_info.ppEnabledLayerNames = config_.validation_layers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }
  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device");
  }

  vkGetDeviceQueue(device_, queue_family_indices_.graphicsFamily.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.presentFamily.value(), 0,
                   &present_queue_);
}

void VulkanRendererAPI::CreateSwapChain() {
  SwapChainSupportDetails swap_chain_support =
      SwapChainSupport(physical_device_);

  VkSurfaceFormatKHR surface_format =
      ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode =
      ChooseSwapPresentMode(swap_chain_support.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (image_count > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface_;
  createInfo.minImageCount = image_count;
  createInfo.imageFormat = surface_format.format;
  createInfo.imageColorSpace = surface_format.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // also can be
                                           // VK_IMAGE_USAGE_TRANSFER_DST_BIT

  // QueueFamilyIndices indicies = FindQueueFamilies(physicalDevice);
  uint32_t queueFamilyIndices[] = {queue_family_indices_.graphicsFamily.value(),
                                   queue_family_indices_.presentFamily.value()};
  if (queue_family_indices_.graphicsFamily !=
      queue_family_indices_.presentFamily) {
    createInfo.imageSharingMode =
        VK_SHARING_MODE_CONCURRENT; // Images can be used across multiple queue
                                    // families without explicit ownership
                                    // transfers.
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode =
        VK_SHARING_MODE_EXCLUSIVE; // An image is owned by one queue family at a
                                   // time and ownership must be explicitly
                                   // transfered before using it in another
                                   // queue family. This option offers the best
                                   // performance.
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }
  createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = present_mode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swap_chain_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain");
  }

  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count,
                          swap_chain_images_.data());
  swap_chain_format_ = surface_format.format;
  swap_chain_extent_ = extent;
}

void VulkanRendererAPI::DestroySwapChain() {
  vkDestroyImageView(device_, depth_image_view_, nullptr);
  vkDestroyImage(device_, depth_image_, nullptr);
  vkFreeMemory(device_, depth_image_memory_, nullptr);
  for (auto &swap_chain_framebuffer : swap_chain_framebuffers_)
    vkDestroyFramebuffer(device_, swap_chain_framebuffer, nullptr);
  vkFreeCommandBuffers(device_, command_pool_,
                       static_cast<uint32_t>(command_buffers_.size()),
                       command_buffers_.data());
  vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  vkDestroyRenderPass(device_, render_pass_, nullptr);
  for (size_t i(0); i < swap_chain_image_views_.size(); ++i)
    vkDestroyImageView(device_, swap_chain_image_views_[i], nullptr);
  vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    vkDestroyBuffer(device_, uniform_buffers_[i], nullptr);
    vkFreeMemory(device_, uniform_buffers_memory_[i], nullptr);
  }
  vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
}

void VulkanRendererAPI::RecreateSwapChain() {
  vkDeviceWaitIdle(device_);
  DestroySwapChain();
  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateGraphicsPipeline();
  CreateDepthResources();
  CreateFramebuffers();
  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateDescriptorSets();
  CreateCommandBuffers();
  UpdateCommandBuffers();
}

void VulkanRendererAPI::CreateImageViews() {
  swap_chain_image_views_.resize(swap_chain_images_.size());

  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    swap_chain_image_views_[i] = CreateImageView(
        swap_chain_images_[i], swap_chain_format_, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}

void VulkanRendererAPI::CreateRenderPass() {
  VkAttachmentDescription color_attachment = {};
  color_attachment.format = swap_chain_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth_attachment = {};
  depth_attachment.format = FindDepthFormat();
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

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachemnts = {color_attachment,
                                                        depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachemnts.size());
  render_pass_info.pAttachments = attachemnts.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass");
  }
}

void VulkanRendererAPI::CreateDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding ubo_layout_binding = {};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  ubo_layout_binding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding sampler_layout_binding = {};
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  sampler_layout_binding.pImmutableSamplers = nullptr;

  std::array bindings = {ubo_layout_binding, sampler_layout_binding};

  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
  layout_info.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr,
                                  &descriptor_set_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout");
  }
}

void VulkanRendererAPI::CreateGraphicsPipeline() {
  // reading code from files
  auto vertex_shader_code =
      readShader("/home/antaraz/Projects/SlashEngine/src/Shaders/vert.spv");
  auto fragment_shader_code =
      readShader("/home/antaraz/Projects/SlashEngine/src/Shaders/frag.spv");

  // creating modules from code
  VkShaderModule vertex_shader_module = CreateShaderModule(vertex_shader_code);
  VkShaderModule fragment_shader_module =
      CreateShaderModule(fragment_shader_code);

  // vertex stage info
  VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
  vert_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vertex_shader_module;
  vert_shader_stage_info.pName = "main";
  vert_shader_stage_info.pSpecializationInfo = nullptr;

  // fragment stage info
  VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
  frag_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = fragment_shader_module;
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = nullptr;

  // array of pipeline stage infos
  VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info,
                                                     frag_shader_stage_info};

  // TMP
  auto binding_descriptions = VulkanVertex::getBindingDescription();
  auto attribute_descriptions = VulkanVertex::getAttributeDescriptions();

  // info of data that is passed to vertex shader
  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &binding_descriptions;
  vertex_input_info.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexAttributeDescriptions =
      attribute_descriptions.data();

  // type of geometry will be drawn
  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swap_chain_extent_.width);
  viewport.height = static_cast<float>(swap_chain_extent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_extent_;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  // VkDynamicState dynamicStates[] = {
  //     VK_DYNAMIC_STATE_VIEWPORT,
  //     VK_DYNAMIC_STATE_LINE_WIDTH,
  // };

  // VkPipelineDynamicStateCreateInfo dynamicState = {};
  // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  // dynamicState.dynamicStateCount = 2;
  // dynamicState.pDynamicStates = dynamicStates;

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
  depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.minDepthBounds = 0.0f;
  depth_stencil.maxDepthBounds = 1.0f;
  depth_stencil.stencilTestEnable = VK_FALSE;
  depth_stencil.front = {};
  depth_stencil.back = {};

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
  pipeline_layout_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr,
                             &pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewportState;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = &depth_stencil;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = nullptr;
  pipeline_info.layout = pipeline_layout_;
  pipeline_info.renderPass = render_pass_;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info,
                                nullptr, &graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline");
  }

  vkDestroyShaderModule(device_, vertex_shader_module, nullptr);
  vkDestroyShaderModule(device_, fragment_shader_module, nullptr);
}

void VulkanRendererAPI::CreateFramebuffers() {
  swap_chain_framebuffers_.resize(swap_chain_image_views_.size());
  for (size_t i(0); i < swap_chain_image_views_.size(); ++i) {
    std::array<VkImageView, 2> attachments = {swap_chain_image_views_[i],
                                              depth_image_view_};
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount =
        static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;
    if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr,
                            &swap_chain_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer");
    }
  }
}

void VulkanRendererAPI::CreateCommandPool() {
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_indices_.graphicsFamily.value();
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool");
  }
}

void VulkanRendererAPI::CreateDepthResources() {
  VkFormat depthFormat = FindDepthFormat();
  CreateImage(
      swap_chain_extent_.width, swap_chain_extent_.height, depthFormat,
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_image_, depth_image_memory_);
  depth_image_view_ =
      CreateImageView(depth_image_, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  TransitionImageLayout(depth_image_, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void VulkanRendererAPI::CreateTextureImage() {
  int texWidth, texHeight, texChannel;
  stbi_uc *pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight,
                              &texChannel, STBI_rgb_alpha);
  VkDeviceSize image_size = texWidth * texHeight * 4;
  if (!pixels) {
    throw std::runtime_error("failed to load texture image");
  }
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               staging_buffer, staging_buffer_memory);
  void *data;
  vkMapMemory(device_, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(image_size));
  vkUnmapMemory(device_, staging_buffer_memory);
  stbi_image_free(pixels);

  CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image_,
              texture_image_memory_);
  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(staging_buffer, texture_image_,
                    static_cast<uint32_t>(texWidth),
                    static_cast<uint32_t>(texHeight));
  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void VulkanRendererAPI::CreateTextureImageView() {
  texture_image_view_ = CreateImageView(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                                        VK_IMAGE_ASPECT_COLOR_BIT);
}

void VulkanRendererAPI::CreateTextureSampler() {
  VkSamplerCreateInfo sampler_info = {};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_TRUE;
  sampler_info.maxAnisotropy = 16;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  if (vkCreateSampler(device_, &sampler_info, nullptr, &texture_sampler_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler");
  }
}

void VulkanRendererAPI::CreateVertexBuffer() {
  //  VkDeviceSize buffer_size =
  //      sizeof(global_vertices[0]) * global_vertices.size();
  //  VkBuffer staging_buffer;
  //  VkDeviceMemory staging_buffer_memory;
  //  CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //               staging_buffer, staging_buffer_memory);
  //  void *data;
  //  vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  //  memcpy(data, global_vertices.data(), static_cast<size_t>(buffer_size));
  //  vkUnmapMemory(device_, staging_buffer_memory);
  //  CreateBuffer(buffer_size,
  //               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
  //                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  //               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_,
  //               vertex_buffer_memory_);
  //  CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);
  //  vkDestroyBuffer(device_, staging_buffer, nullptr);
  //  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void VulkanRendererAPI::CreateIndexBuffer() {
  //  VkDeviceSize buffer_size = sizeof(global_indices[0]) *
  //  global_indices.size(); VkBuffer staging_buffer; VkDeviceMemory
  //  staging_buffer_memory; CreateBuffer(buffer_size,
  //  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //               staging_buffer, staging_buffer_memory);
  //  void *data;
  //  vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  //  memcpy(data, global_indices.data(), static_cast<size_t>(buffer_size));
  //  vkUnmapMemory(device_, staging_buffer_memory);
  //  CreateBuffer(
  //      buffer_size,
  //      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
  //      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_,
  //      index_buffer_memory_);
  //  CopyBuffer(staging_buffer, index_buffer_, buffer_size);
  //  vkDestroyBuffer(device_, staging_buffer, nullptr);
  //  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void VulkanRendererAPI::CreateUniformBuffers() {
  VkDeviceSize buffer_size = sizeof(UniformBufferObject);
  uniform_buffers_.resize(swap_chain_images_.size());
  uniform_buffers_memory_.resize(swap_chain_image_views_.size());
  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 uniform_buffers_[i], uniform_buffers_memory_[i]);
  }
}

void VulkanRendererAPI::CreateDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount =
      static_cast<uint32_t>(swap_chain_images_.size());
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount =
      static_cast<uint32_t>(swap_chain_images_.size());
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = static_cast<uint32_t>(swap_chain_images_.size());
  if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &descriptor_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool");
  }
}

void VulkanRendererAPI::CreateDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(swap_chain_images_.size(),
                                             descriptor_set_layout_);
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool_;
  alloc_info.descriptorSetCount =
      static_cast<uint32_t>(swap_chain_images_.size());
  alloc_info.pSetLayouts = layouts.data();
  descriptor_sets_.resize(swap_chain_images_.size());
  if (vkAllocateDescriptorSets(device_, &alloc_info, descriptor_sets_.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets");
  }
  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = uniform_buffers_[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture_image_view_;
    image_info.sampler = texture_sampler_;

    std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_sets_[i];
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = descriptor_sets_[i];
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pBufferInfo = nullptr;
    descriptor_writes[1].pImageInfo = &image_info;
    descriptor_writes[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device_,
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(), 0, nullptr);
  }
}

void VulkanRendererAPI::CreateCommandBuffers() {
  command_buffers_.resize(swap_chain_framebuffers_.size());
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount =
      static_cast<uint32_t>(command_buffers_.size());
  if (vkAllocateCommandBuffers(device_, &alloc_info, command_buffers_.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command buffers");
  }

  for (size_t i(0); i < command_buffers_.size(); ++i) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer");
    }
    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
  }
}

void VulkanRendererAPI::UpdateCommandBuffers() {
  if (!buffer_manager_->HasData())
    return;
  for (size_t i(0); i < command_buffers_.size(); ++i) {
    vkResetCommandBuffer(command_buffers_[i], 0);
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer");
    }
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass_;
    render_pass_info.framebuffer = swap_chain_framebuffers_[i];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_extent_;
    std::array<VkClearValue, 2> clear_values = {};
    clear_values[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clear_values[1].depthStencil = {1.0f, 0};
    render_pass_info.clearValueCount =
        static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();
    vkCmdBeginRenderPass(command_buffers_[i], &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphics_pipeline_);
    for (size_t j(0); j < buffer_manager_->Size(); ++j) {
      vkCmdBindVertexBuffers(command_buffers_[i], 0, 1,
                             buffer_manager_->GetVertexBuffer(j),
                             buffer_manager_->GetVertexBuffersOffsets(j));
      vkCmdBindIndexBuffer(command_buffers_[i],
                           buffer_manager_->GetIndexBuffer(j), 0,
                           VK_INDEX_TYPE_UINT16);
      vkCmdBindDescriptorSets(command_buffers_[i],
                              VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_,
                              0, 1, &descriptor_sets_[i], 0, nullptr);
      vkCmdDrawIndexed(command_buffers_[i],
                       buffer_manager_->GetIndexBufferSize(j), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(command_buffers_[i]);
    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
  }
}

void VulkanRendererAPI::CreateSemaphores() {
  image_available_semaphores_.resize(config_.k_max_frames_in_flight);
  render_finished_semaphores_.resize(config_.k_max_frames_in_flight);
  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for (size_t i(0); i < config_.k_max_frames_in_flight; ++i) {
    if (vkCreateSemaphore(device_, &semaphore_info, nullptr,
                          &image_available_semaphores_[i]) ||
        vkCreateSemaphore(device_, &semaphore_info, nullptr,
                          &render_finished_semaphores_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores for frame");
    }
  }
}
void VulkanRendererAPI::CreateFences() {
  inflight_fences_.resize(config_.k_max_frames_in_flight);
  images_inflight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);
  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (size_t i(0); i < config_.k_max_frames_in_flight; ++i) {
    if (vkCreateFence(device_, &fence_info, nullptr, &inflight_fences_[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create fence");
    }
  }
}

//////////////////////////////////////////////////////
/////////// ADITIONAL FUNC ///////////////////////////
//////////////////////////////////////////////////////

// Get requared Vulkan instance extensions required by GLFW
std::vector<const char *> VulkanRendererAPI::GetRequaredExtentions() {
  uint32_t extensions_count = 0;
  const char **extensions;
  extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
  std::vector<const char *> return_extensions(extensions,
                                              extensions + extensions_count);
  if (config_.enable_validation_layers)
    return_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  return return_extensions;
}

// Debug callback for logging vulkan messages
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererAPI::DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallBackData,
    [[maybe_unused]] void *pUserData) {
  switch (messageSeverity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    SL_CORE_TRACE(pCallBackData->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    SL_CORE_INFO(pCallBackData->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    SL_CORE_WARN(pCallBackData->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    SL_CORE_ERROR(pCallBackData->pMessage);
    break;
  default:
    SL_CORE_ERROR(pCallBackData->pMessage);
  }
  return VK_FALSE;
}

void VulkanRendererAPI::SetUpDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      //                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
      //                                     |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallBack;
  createInfo.pUserData = nullptr;
}

VkResult VulkanRendererAPI::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void VulkanRendererAPI::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

// Checks physical device if it has needed properties, features and queue
// families
bool VulkanRendererAPI::IsDeviceSuitable(const VkPhysicalDevice &device) {
  // TODO make props and features be choosable
  // Getting device properties
  // VkPhysicalDeviceProperties deviceProperties;
  // vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);

  // Find apropriet queue families for device (specified in queue family
  // property)
  QueueFamilyIndices indices = FindQueueFamilies(device);
  bool extentions_supported = CheckDeviceExtensionSupport(device);
  bool swapChainAdequate = false;
  if (extentions_supported) {
    auto swapChainSupport = SwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  // TODO make this configurable
  return indices.IsComplete() && extentions_supported && swapChainAdequate &&
         supported_features.samplerAnisotropy;
}

// Gets queue family that has particular properties
VulkanRendererAPI::QueueFamilyIndices
VulkanRendererAPI::FindQueueFamilies(const VkPhysicalDevice &device) {
  QueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queueFamilies.data());

  // Pick needed queue family
  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueCount > config_.queue_family_config.min_queue_count &&
        queueFamily.queueFlags & config_.queue_family_config.queue_flags) {
      indices.graphicsFamily = i;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);

    if (queueFamily.queueCount > config_.queue_family_config.min_queue_count &&
        present_support) {
      indices.presentFamily = i;
    }
    if (indices.IsComplete()) {
      break;
    }
    ++i;
  }
  return indices;
}

bool VulkanRendererAPI::CheckDeviceExtensionSupport(
    const VkPhysicalDevice &device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       available_extensions.data());

  std::set<std::string_view> required_extensions(
      config_.device_extentions.begin(), config_.device_extentions.end());
  for (const auto &extention : available_extensions) {
    required_extensions.erase(extention.extensionName);
  }
  return required_extensions.empty();
}

VulkanRendererAPI::SwapChainSupportDetails
VulkanRendererAPI::SwapChainSupport(const VkPhysicalDevice &device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_,
                                            &details.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count,
                                       nullptr);
  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count,
                                         details.formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_,
                                            &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    details.presentModes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface_, &present_mode_count, details.presentModes.data());
  }
  return details;
}

VkSurfaceFormatKHR VulkanRendererAPI::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &available_format : availableFormats) {
    if (available_format.format == config_.swap_chain_config.format &&
        available_format.colorSpace == config_.swap_chain_config.color_space) {
      return available_format;
    }
  }
  return availableFormats[0];
}

VkPresentModeKHR VulkanRendererAPI::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &available_present_mode : availablePresentModes) {
    if (available_present_mode == config_.swap_chain_config.present_mode) {
      return available_present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRendererAPI::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(
        static_cast<GLFWwindow *>(
            slash::Application::Get().GetWindow().GetNativeWindow()),
        &width, &height);
    VkExtent2D actual_extent = {static_cast<uint32_t>(width),
                                static_cast<uint32_t>(height)};
    actual_extent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.width = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
  }
}

std::vector<char> VulkanRendererAPI::readShader(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open file");
  }
  size_t size = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(size);
  file.seekg(0);
  file.read(buffer.data(), size);
  return buffer;
}

VkShaderModule
VulkanRendererAPI::CreateShaderModule(const std::vector<char> &code) {
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device_, &create_info, nullptr, &shader_module) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
  return shader_module;
}

uint32_t VulkanRendererAPI::FindMemoryType(uint32_t typeFilter,
                                           VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties);
  for (uint32_t i(0); i < memory_properties.memoryTypeCount; ++i) {
    if (typeFilter & (1 << i) &&
        (memory_properties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type");
}

void VulkanRendererAPI::CreateBuffer(VkDeviceSize size,
                                     VkBufferUsageFlags usage,
                                     VkMemoryPropertyFlags properties,
                                     VkBuffer &buffer,
                                     VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer");
  }

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(device_, buffer, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex =
      FindMemoryType(mem_requirements.memoryTypeBits, properties);
  if (vkAllocateMemory(device_, &alloc_info, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory");
  }
  vkBindBufferMemory(device_, buffer, bufferMemory, 0);
}

void VulkanRendererAPI::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                                   VkDeviceSize size) {
  auto command_buffer = BeginSingleTimeCommand();
  VkBufferCopy copyRegion = {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;
  vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);
  EndSingleTimeCommand(command_buffer);
}

void VulkanRendererAPI::UpdateUniformBuffer(float time, uint32_t currentImage) {
  static float time_passed = 0;
  time_passed += time;
  UniformBufferObject ubo = {};
  ubo.model = glm::rotate(glm::mat4(1.0f), time_passed * glm::radians(90.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f),
                              static_cast<float>(swap_chain_extent_.width) /
                                  static_cast<float>(swap_chain_extent_.height),
                              0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
  void *data;
  vkMapMemory(device_, uniform_buffers_memory_[currentImage], 0, sizeof(ubo), 0,
              &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(device_, uniform_buffers_memory_[currentImage]);
}

void VulkanRendererAPI::CreateImage(uint32_t width, uint32_t height,
                                    VkFormat format, VkImageTiling tiling,
                                    VkImageUsageFlags usage,
                                    VkMemoryPropertyFlags properties,
                                    VkImage &image,
                                    VkDeviceMemory &imageMemory) {
  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image");
  }

  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(device_, image, &memory_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = memory_requirements.size;
  alloc_info.memoryTypeIndex =
      FindMemoryType(memory_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_, &alloc_info, nullptr, &imageMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate memory");
  }

  vkBindImageMemory(device_, image, imageMemory, 0);
}

VkCommandBuffer VulkanRendererAPI::BeginSingleTimeCommand() {
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool_;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer, &begin_info);
  return command_buffer;
}

void VulkanRendererAPI::EndSingleTimeCommand(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue_);

  vkFreeCommandBuffers(device_, command_pool_, 1, &commandBuffer);
}

void VulkanRendererAPI::TransitionImageLayout(VkImage image,
                                              [[maybe_unused]] VkFormat format,
                                              VkImageLayout oldLayout,
                                              VkImageLayout newLayout) {
  auto command_buffer = BeginSingleTimeCommand();

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
        format == VK_FORMAT_D24_UNORM_S8_UINT) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::runtime_error("unsupported layout transition");
  }

  vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  EndSingleTimeCommand(command_buffer);
}

void VulkanRendererAPI::CopyBufferToImage(VkBuffer buffer, VkImage image,
                                          uint32_t width, uint32_t height) {
  auto command_buffer = BeginSingleTimeCommand();
  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};
  vkCmdCopyBufferToImage(command_buffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  EndSingleTimeCommand(command_buffer);
}

VkImageView VulkanRendererAPI::CreateImageView(VkImage image, VkFormat format,
                                               VkImageAspectFlags aspectFlags) {
  VkImageViewCreateInfo view_info = {};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.subresourceRange.aspectMask = aspectFlags;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  VkImageView image_view;
  if (vkCreateImageView(device_, &view_info, nullptr, &image_view) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture views!");
  }
  return image_view;
}

VkFormat
VulkanRendererAPI::FindSupportedFormat(const std::vector<VkFormat> &candidates,
                                       VkImageTiling tiling,
                                       VkFormatFeatureFlags features) {
  for (auto format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);
    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format");
}

VkFormat VulkanRendererAPI::FindDepthFormat() {
  return FindSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D32_SFLOAT_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

// Checks availability of validation layers
bool VulkanRendererAPI::CheckValidationLayers() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> availableLayers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());
  for (const auto &valLayer : config_.validation_layers) {
    bool layers_found = false;
    for (const auto &layer : availableLayers) {
      if (strcmp(valLayer, layer.layerName) == 0) {
        layers_found = true;
        break;
      }
    }
    if (!layers_found)
      return false;
  }
  return true;
}

/////////////////////////////////
////// VulkanBufferManager //////
/////////////////////////////////

VulkanBufferManager::VulkanBufferManager(VulkanRendererAPI *renderer_api)
    : renderer_api_(renderer_api) {}

VulkanBufferManager::~VulkanBufferManager() {
  for (auto &index : buffer_uids_) {
    RemoveVertexBuffer(index);
    RemoveIndexBuffer(index);
  }
}

void VulkanBufferManager::AddVertexBuffer(size_t uid,
                                          const std::vector<Vertex> &vertices) {
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;
  VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  renderer_api_->CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              staging_buffer, staging_buffer_memory);
  void *data;
  vkMapMemory(renderer_api_->device_, staging_buffer_memory, 0, buffer_size, 0,
              &data);
  memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
  vkUnmapMemory(renderer_api_->device_, staging_buffer_memory);
  renderer_api_->CreateBuffer(
      buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer, vertex_buffer_memory);
  renderer_api_->CopyBuffer(staging_buffer, vertex_buffer, buffer_size);
  vkDestroyBuffer(renderer_api_->device_, staging_buffer, nullptr);
  vkFreeMemory(renderer_api_->device_, staging_buffer_memory, nullptr);
  vertex_buffers_.insert({uid, vertex_buffer});
  vertex_buffers_memory_.insert({uid, vertex_buffer_memory});
  vertex_offsets_.insert({uid, 0});
  buffer_uids_.push_back(uid);
}

void VulkanBufferManager::AddIndexBuffer([[maybe_unused]] size_t uid,
                                         const std::vector<uint16_t> &indices) {
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
  VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  renderer_api_->CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              staging_buffer, staging_buffer_memory);
  void *data;
  vkMapMemory(renderer_api_->device_, staging_buffer_memory, 0, buffer_size, 0,
              &data);
  memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
  vkUnmapMemory(renderer_api_->device_, staging_buffer_memory);
  renderer_api_->CreateBuffer(
      buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer, index_buffer_memory);
  renderer_api_->CopyBuffer(staging_buffer, index_buffer, buffer_size);
  vkDestroyBuffer(renderer_api_->device_, staging_buffer, nullptr);
  vkFreeMemory(renderer_api_->device_, staging_buffer_memory, nullptr);
  index_buffers_.insert({uid, index_buffer});
  index_buffers_memory_.insert({uid, index_buffer_memory});
  index_buffers_size_.insert({uid, static_cast<uint32_t>(indices.size())});
}

bool VulkanBufferManager::HasData() const { return !buffer_uids_.empty(); }

size_t VulkanBufferManager::Size() const { return buffer_uids_.size(); }

void VulkanBufferManager::RemoveVertexBuffer(size_t uid) {
  vkDestroyBuffer(renderer_api_->device_, vertex_buffers_[uid], nullptr);
  vkFreeMemory(renderer_api_->device_, vertex_buffers_memory_[uid], nullptr);
  vertex_buffers_.erase(uid);
  vertex_buffers_memory_.erase(uid);
  vertex_offsets_.erase(uid);
}

void VulkanBufferManager::RemoveIndexBuffer(size_t uid) {
  vkDestroyBuffer(renderer_api_->device_, index_buffers_[uid], nullptr);
  vkFreeMemory(renderer_api_->device_, index_buffers_memory_[uid], nullptr);
  index_buffers_.erase(uid);
  index_buffers_memory_.erase(uid);
  index_buffers_size_.erase(uid);
}

const VkBuffer *VulkanBufferManager::GetVertexBuffer(size_t index) const {
  return &vertex_buffers_.at(buffer_uids_[index]);
}

const VkDeviceSize *
VulkanBufferManager::GetVertexBuffersOffsets(size_t index) const {
  return &vertex_offsets_.at(buffer_uids_[index]);
}

VkBuffer VulkanBufferManager::GetIndexBuffer(size_t index) const {
  return index_buffers_.at(buffer_uids_[index]);
}

uint32_t VulkanBufferManager::GetIndexBufferSize(size_t index) const {
  return index_buffers_size_.at(buffer_uids_[index]);
}

} // namespace slash