#include "VulkanRendererAPI.hpp"
#include "Core/application.hpp"
#include "Core/log.hpp"
#include "slash_pch.hpp"

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

void VulkanRendererAPI::Init() {
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
  CreateVertexBuffer();
  CreateIndexBuffer();
  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateDescriptorSets();
  CreateCommandBuffers();
  CreateSemaphores();
  CreateFences();
}

void VulkanRendererAPI::Destroy() {
  vkDeviceWaitIdle(device_);
  for (size_t i(0); i < MAX_FRAMES_IN_FLIGHT; ++i) {
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
  vkDestroyBuffer(device_, vertex_buffer_, nullptr);
  vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
  vkDestroyBuffer(device_, index_buffer_, nullptr);
  vkFreeMemory(device_, index_buffer_memory_, nullptr);
  vkDestroyCommandPool(device_, command_pool_, nullptr);
  vkDestroyDevice(device_, nullptr);
  if (config_.enableValidationLayers)
    DestroyDebugUtilsMessengerEXT(instance_, debug_utils_messenger_ext_, nullptr);
  vkDestroySurfaceKHR(instance_, surface_khr_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

void VulkanRendererAPI::DrawFrame(float time) {
  vkWaitForFences(device_, 1, &inflight_fences_[current_frame], VK_TRUE,
                  UINT64_MAX);
  uint32_t imageIndex;
  auto result = vkAcquireNextImageKHR(device_, swap_chain_, UINT64_MAX,
      image_available_semaphores_[current_frame],
                                      VK_NULL_HANDLE, &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    window_data_->resized = false;
    RecreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }
  if (images_inflight_[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(device_, 1, &images_inflight_[imageIndex], VK_TRUE,
                    UINT64_MAX);
  }
  // imagesInFlight[imageIndex] = inFlightFences[current_frame];
  UpdateUniformBuffer(time, imageIndex);
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {image_available_semaphores_[current_frame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffers_[imageIndex];
  VkSemaphore signalSemaphores[] = {render_finished_semaphores_[current_frame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;
  vkResetFences(device_, 1, &inflight_fences_[current_frame]);
  if (vkQueueSubmit(graphics_queue_, 1, &submitInfo,
                    inflight_fences_[current_frame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer");
  }
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {swap_chain_};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;
  result = vkQueuePresentKHR(present_queue_, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window_data_->resized) {
    window_data_->resized = false;
    RecreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image");
  }
  current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//////////////////////////////////////////////////////
/////////// PRIVATE FUNC /////////////////////////////
//////////////////////////////////////////////////////

void VulkanRendererAPI::CreateInstance() {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Vulkan app";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.pEngineName = "Slash";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensionsProps(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensionsProps.data());

  SL_CORE_INFO("Available extentions: ");
  for (const auto &ext : extensionsProps) {
    SL_CORE_INFO(ext.extensionName);
  }

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = GetRequaredExtentions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  if (config_.enableValidationLayers && !CheckValidationLayers()) {
    throw std::runtime_error("validation layers requested? but not available");
  }
  if (config_.enableValidationLayers && CheckValidationLayers()) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validation_layers_.size());
    createInfo.ppEnabledLayerNames = validation_layers_.data();
    SetUpDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan instance");
  }
}

void VulkanRendererAPI::SetUpDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT debugInfo;
  SetUpDebugMessengerCreateInfo(debugInfo);

  if (CreateDebugUtilsMessengerEXT(instance_, &debugInfo, nullptr,
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
          nullptr, &surface_khr_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create widow surface");
  }
}

// Get physical GPU in the system
void VulkanRendererAPI::PickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

  for (const auto &device : devices) {
    // TODO make some GPU score to pick one with most performance
    if (IsDeviceSutable(device)) {
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
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
      queue_family_indices_.graphicsFamily.value(),
      queue_family_indices_.presentFamily.value()};

  float queuePriority = 1.0f; // must be between 0 and 1
  for ([[maybe_unused]] uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex =
        queue_family_indices_.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {}; // features that will be in use
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(device_extentions_.size());
  createInfo.ppEnabledExtensionNames = device_extentions_.data();

  if (config_.enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validation_layers_.size());
    createInfo.ppEnabledLayerNames = validation_layers_.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
  if (vkCreateDevice(physical_device_, &createInfo, nullptr, &device_) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device");
  }

  vkGetDeviceQueue(device_, queue_family_indices_.graphicsFamily.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.presentFamily.value(), 0,
                   &present_queue_);
}

void VulkanRendererAPI::CreateSwapChain() {
  SwapChainSupportDetails swapChainSupport = SwapChainSupport(physical_device_);

  VkSurfaceFormatKHR surfaceFormat =
      ChooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      ChooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (imageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface_khr_;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
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
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swap_chain_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain");
  }

  vkGetSwapchainImagesKHR(device_, swap_chain_, &imageCount, nullptr);
  swap_chain_images_.resize(imageCount);
  vkGetSwapchainImagesKHR(device_, swap_chain_, &imageCount,
                          swap_chain_images_.data());
  swap_chain_format_ = surfaceFormat.format;
  swap_chain_extent_ = extent;
}

void VulkanRendererAPI::DestroySwapChain() {
  vkDestroyImageView(device_, depth_image_view_, nullptr);
  vkDestroyImage(device_, depth_image_, nullptr);
  vkFreeMemory(device_, depth_image_memory_, nullptr);
  for (size_t i(0); i < swap_chain_framebuffers_.size(); ++i)
    vkDestroyFramebuffer(device_, swap_chain_framebuffers_[i], nullptr);
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
}

void VulkanRendererAPI::CreateImageViews() {
  swap_chain_image_views_.resize(swap_chain_images_.size());

  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    swap_chain_image_views_[i] = CreateImageView(
        swap_chain_images_[i], swap_chain_format_, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}

void VulkanRendererAPI::CreateRenderPass() {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = swap_chain_format_;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = FindDepthFormat();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachemnts = {colorAttachment,
                                                        depthAttachment};
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachemnts.size());
  renderPassInfo.pAttachments = attachemnts.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device_, &renderPassInfo, nullptr, &render_pass_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass");
  }
}

void VulkanRendererAPI::CreateDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array bindings = {uboLayoutBinding, samplerLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr,
                                  &descriptor_set_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout");
  }
}

void VulkanRendererAPI::CreateGraphicsPipeline() {
  // reading code from files
  auto vertShaderCode =
      readShader("/home/antaraz/Projects/SlashEngine/src/Shaders/vert.spv");
  auto fragShaderCode =
      readShader("/home/antaraz/Projects/SlashEngine/src/Shaders/frag.spv");

  // creating modules from code
  VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
  VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

  // vertex stage info
  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";
  vertShaderStageInfo.pSpecializationInfo = nullptr;

  // fragment stage info
  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";
  fragShaderStageInfo.pSpecializationInfo = nullptr;

  // array of pipeline stage infos
  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  // TMP
  auto bindingDesctription = VulkanVertex::getBindingDescription();
  auto attributeDescriptions = VulkanVertex::getAttributeDescriptions();

  // info of data that is passed to vertex shader
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDesctription;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  // type of geometry will be drawn
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

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

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  // VkDynamicState dynamicStates[] = {
  //     VK_DYNAMIC_STATE_VIEWPORT,
  //     VK_DYNAMIC_STATE_LINE_WIDTH,
  // };

  // VkPipelineDynamicStateCreateInfo dynamicState = {};
  // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  // dynamicState.dynamicStateCount = 2;
  // dynamicState.pDynamicStates = dynamicStates;

  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f;
  depthStencil.maxDepthBounds = 1.0f;
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {};
  depthStencil.back = {};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &descriptor_set_layout_;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr,
                             &pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = nullptr;
  pipelineInfo.layout = pipeline_layout_;
  pipelineInfo.renderPass = render_pass_;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline");
  }

  vkDestroyShaderModule(device_, vertShaderModule, nullptr);
  vkDestroyShaderModule(device_, fragShaderModule, nullptr);
}

void VulkanRendererAPI::CreateFramebuffers() {
  swap_chain_framebuffers_.resize(swap_chain_image_views_.size());
  for (size_t i(0); i < swap_chain_image_views_.size(); ++i) {
    std::array<VkImageView, 2> attachments = {swap_chain_image_views_[i],
                                              depth_image_view_};
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = render_pass_;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swap_chain_extent_.width;
    framebufferInfo.height = swap_chain_extent_.height;
    framebufferInfo.layers = 1;
    if (vkCreateFramebuffer(device_, &framebufferInfo, nullptr,
                            &swap_chain_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer");
    }
  }
}

void VulkanRendererAPI::CreateCommandPool() {
  // QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queue_family_indices_.graphicsFamily.value();
  poolInfo.flags = 0;
  if (vkCreateCommandPool(device_, &poolInfo, nullptr, &command_pool_) !=
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
  VkDeviceSize imageSize = texWidth * texHeight * 4;
  if (!pixels) {
    throw std::runtime_error("failed to load texture image");
  }
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);
  void *data;
  vkMapMemory(device_, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device_, stagingBufferMemory);
  stbi_image_free(pixels);

  CreateImage(
      texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image_,
              texture_image_memory_);
  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(stagingBuffer, texture_image_,
                    static_cast<uint32_t>(texWidth),
                    static_cast<uint32_t>(texHeight));
  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device_, stagingBuffer, nullptr);
  vkFreeMemory(device_, stagingBufferMemory, nullptr);
}

void VulkanRendererAPI::CreateTextureImageView() {
  texture_image_view_ = CreateImageView(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                                     VK_IMAGE_ASPECT_COLOR_BIT);
}

void VulkanRendererAPI::CreateTextureSampler() {
  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  if (vkCreateSampler(device_, &samplerInfo, nullptr, &texture_sampler_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler");
  }
}

void VulkanRendererAPI::CreateVertexBuffer() {
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);
  void *data;
  vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(device_, stagingBufferMemory);
  CreateBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_,
               vertex_buffer_memory_);
  CopyBuffer(stagingBuffer, vertex_buffer_, bufferSize);
  vkDestroyBuffer(device_, stagingBuffer, nullptr);
  vkFreeMemory(device_, stagingBufferMemory, nullptr);
}

void VulkanRendererAPI::CreateIndexBuffer() {
  VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);
  void *data;
  vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(device_, stagingBufferMemory);
  CreateBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);
  CopyBuffer(stagingBuffer, index_buffer_, bufferSize);
  vkDestroyBuffer(device_, stagingBuffer, nullptr);
  vkFreeMemory(device_, stagingBufferMemory, nullptr);
}

void VulkanRendererAPI::CreateUniformBuffers() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);
  uniform_buffers_.resize(swap_chain_images_.size());
  uniform_buffers_memory_.resize(swap_chain_image_views_.size());
  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 uniform_buffers_[i], uniform_buffers_memory_[i]);
  }
}

void VulkanRendererAPI::CreateDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2> poolSizes = {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(swap_chain_images_.size());
  if (vkCreateDescriptorPool(device_, &poolInfo, nullptr, &descriptor_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool");
  }
}

void VulkanRendererAPI::CreateDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(swap_chain_images_.size(),
                                             descriptor_set_layout_);
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptor_pool_;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(swap_chain_images_.size());
  allocInfo.pSetLayouts = layouts.data();
  descriptor_sets_.resize(swap_chain_images_.size());
  if (vkAllocateDescriptorSets(device_, &allocInfo, descriptor_sets_.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets");
  }
  for (size_t i(0); i < swap_chain_images_.size(); ++i) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniform_buffers_[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture_image_view_;
    imageInfo.sampler = texture_sampler_;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptor_sets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;
    descriptorWrites[0].pImageInfo = nullptr;
    descriptorWrites[0].pTexelBufferView = nullptr;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptor_sets_[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = nullptr;
    descriptorWrites[1].pImageInfo = &imageInfo;
    descriptorWrites[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device_,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }
}

void VulkanRendererAPI::CreateCommandBuffers() {
  command_buffers_.resize(swap_chain_framebuffers_.size());
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool_;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());
  if (vkAllocateCommandBuffers(device_, &allocInfo, command_buffers_.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command buffers");
  }

  for (size_t i(0); i < command_buffers_.size(); ++i) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(command_buffers_[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer");
    }
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_pass_;
    renderPassInfo.framebuffer = swap_chain_framebuffers_[i];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swap_chain_extent_;
    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(command_buffers_[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphics_pipeline_);
    VkBuffer vertexBuffers[] = {vertex_buffer_};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command_buffers_[i], index_buffer_, 0,
                         VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout_, 0, 1, &descriptor_sets_[i], 0,
                            nullptr);
    vkCmdDrawIndexed(command_buffers_[i], static_cast<uint32_t>(indices.size()),
                     1, 0, 0, 0);
    vkCmdEndRenderPass(command_buffers_[i]);
    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
  }
}

void VulkanRendererAPI::CreateSemaphores() {
  image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for (size_t i(0); i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr,
                          &image_available_semaphores_[i]) ||
        vkCreateSemaphore(device_, &semaphoreInfo, nullptr,
                          &render_finished_semaphores_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores for frame");
    }
  }
}
void VulkanRendererAPI::CreateFences() {
  inflight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
  images_inflight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (size_t i(0); i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (vkCreateFence(device_, &fenceInfo, nullptr, &inflight_fences_[i]) !=
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
  uint32_t extensionsCount = 0;
  const char **extensions;
  extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
  std::vector<const char *> retExtentions(extensions,
                                          extensions + extensionsCount);
  if (config_.enableValidationLayers)
    retExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  return retExtentions;
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
    // SL_CORE_WARN("For objects: ");
    // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
    //     SL_CORE_WARN(pCallBackData->pObjects[i].pObjectName);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    SL_CORE_ERROR(pCallBackData->pMessage);
    // SL_CORE_ERROR("For objects: ");
    // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
    //     SL_CORE_ERROR(pCallBackData->pObjects[i].pObjectName);
    break;
  default:
    SL_CORE_ERROR(pCallBackData->pMessage);
    // SL_CORE_ERROR("For objects: ");
    // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
    //     SL_CORE_ERROR(pCallBackData->pObjects[i].pObjectName);
  }
  return VK_FALSE;
}

// Confiugures types of messages that will be recived
void VulkanRendererAPI::SetUpDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
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
bool VulkanRendererAPI::IsDeviceSutable(const VkPhysicalDevice &device) {
  // TODO make props and features be choosable
  // Getting device properties
  // VkPhysicalDeviceProperties deviceProperties;
  // vkGetPhysicalDeviceProperties(device, &deviceProperties);

  // // Getting device features
  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

  // Find apropriet queue families for device (specified in queue family
  // property)
  QueueFamilyIndices indices = FindQueueFamilies(device);
  bool extentionsSupported = CheckDeviceExtensionSupport(device);
  bool swapChainAdequate = false;
  if (extentionsSupported) {
    auto swapChainSupport = SwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  // TODO make this configurable
  return indices.IsComplete() && extentionsSupported && swapChainAdequate &&
         supportedFeatures.samplerAnisotropy;
}

// Gets queue family that has particular properties
VulkanRendererAPI::QueueFamilyIndices
VulkanRendererAPI::FindQueueFamilies(const VkPhysicalDevice &device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  // Pick needed queue family
  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueCount > config_.queue_family_config_.minQueueCount &&
        queueFamily.queueFlags & config_.queue_family_config_.queueFlags) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupprt = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_khr_, &presentSupprt);

    if (queueFamily.queueCount > config_.queue_family_config_.minQueueCount &&
        presentSupprt) {
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
  uint32_t extentionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount,
                                       nullptr);
  std::vector<VkExtensionProperties> availableExtentions(extentionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount,
                                       availableExtentions.data());

  std::set<std::string_view> requiredExtentions(
      config_.deviceExtentions.begin(), config_.deviceExtentions.end());
  for (const auto &extention : availableExtentions) {
    requiredExtentions.erase(extention.extensionName);
  }
  return requiredExtentions.empty();
}

VulkanRendererAPI::SwapChainSupportDetails
VulkanRendererAPI::SwapChainSupport(const VkPhysicalDevice &device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_khr_,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_khr_, &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_khr_, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_khr_, &presentModeCount,
                                            nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface_khr_, &presentModeCount, details.presentModes.data());
  }
  return details;
}

VkSurfaceFormatKHR VulkanRendererAPI::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == config_.swap_chain_config_.format &&
        availableFormat.colorSpace == config_.swap_chain_config_.colorSpace) {
      return availableFormat;
    }
  }
  return availableFormats[0];
}

VkPresentModeKHR VulkanRendererAPI::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == config_.swap_chain_config_.presentMode) {
      return availablePresentMode;
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
    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};
    actualExtent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.width = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
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
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
  return shaderModule;
}

uint32_t VulkanRendererAPI::FindMemoryType(uint32_t typeFilter,
                                           VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &memProperties);
  for (uint32_t i(0); i < memProperties.memoryTypeCount; ++i) {
    if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags &
                                  properties) == properties) {
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
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      FindMemoryType(memRequirements.memoryTypeBits, properties);
  if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory");
  }
  vkBindBufferMemory(device_, buffer, bufferMemory, 0);
}

void VulkanRendererAPI::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                                   VkDeviceSize size) {
  auto commandBuffer = BeginSingleTimeCommand();
  VkBufferCopy copyRegion = {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
  EndSingleTimeCommand(commandBuffer);
}

void VulkanRendererAPI::UpdateUniformBuffer(float time, uint32_t currentImage) {
  static float timePassed = 0;
  timePassed += time;
  UniformBufferObject ubo = {};
  ubo.model = glm::rotate(glm::mat4(1.0f), timePassed * glm::radians(90.0f),
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
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device_, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      FindMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate memory");
  }

  vkBindImageMemory(device_, image, imageMemory, 0);
}

VkCommandBuffer VulkanRendererAPI::BeginSingleTimeCommand() {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = command_pool_;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
  return commandBuffer;
}

void VulkanRendererAPI::EndSingleTimeCommand(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphics_queue_, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue_);

  vkFreeCommandBuffers(device_, command_pool_, 1, &commandBuffer);
}

void VulkanRendererAPI::TransitionImageLayout(VkImage image,
                                              [[maybe_unused]] VkFormat format,
                                              VkImageLayout oldLayout,
                                              VkImageLayout newLayout) {
  auto commandBuffer = BeginSingleTimeCommand();

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

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::runtime_error("unsupported layout transition");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  EndSingleTimeCommand(commandBuffer);
}

void VulkanRendererAPI::CopyBufferToImage(VkBuffer buffer, VkImage image,
                                          uint32_t width, uint32_t height) {
  auto commandBuffer = BeginSingleTimeCommand();
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
  vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  EndSingleTimeCommand(commandBuffer);
}

VkImageView VulkanRendererAPI::CreateImageView(VkImage image, VkFormat format,
                                               VkImageAspectFlags aspectFlags) {
  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(device_, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture views!");
  }
  return imageView;
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
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  for (const auto &valLayer : validation_layers_) {
    bool layersFound = false;
    for (const auto &layer : availableLayers) {
      if (strcmp(valLayer, layer.layerName) == 0) {
        layersFound = true;
        break;
      }
    }

    if (!layersFound)
      return false;
  }
  return true;
}
} // namespace slash
