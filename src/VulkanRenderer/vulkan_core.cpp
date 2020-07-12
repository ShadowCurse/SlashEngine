#include "vulkan_core.hpp"
#include "Core/log.hpp"
namespace slash {

VulkanCore::VulkanCore(GLFWwindow *window) : window_(window) {
  CreateInstance();
  CreateSurface();
#if defined(SL_RENDERER_VALIDATION)
  SetUpDebugMessenger();
#endif
  PickPhysicalDevice();
  CreateLogicalDevice();
}

VulkanCore::~VulkanCore() {
  vkDestroyDevice(device_, nullptr);
#if defined(SL_RENDERER_VALIDATION)
  DestroyDebugUtilsMessengerEXT(instance_, debug_utils_messenger_ext_, nullptr);
#endif
  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

const VkDevice &VulkanCore::GetDevice() const { return device_; }
const VkSurfaceKHR &VulkanCore::GetSuface() const { return surface_; }
const VkPhysicalDevice &VulkanCore::GetPhysicalDevice() const { return gpu_; }
QueueFamilyIndices VulkanCore::GetQueueFamilies() const {
  return FindQueueFamilies(gpu_);
}
std::pair<uint32_t, uint32_t> VulkanCore::GetWindowSize() const {
  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

VkFormat
VulkanCore::FindSupportedFormat(const std::vector<VkFormat> &candidates,
                                VkImageTiling tiling,
                                VkFormatFeatureFlags features) {
  for (auto format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(gpu_, format, &props);
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

uint32_t VulkanCore::FindMemoryTypeIndex(uint32_t memory_type_bits,
                                         VkMemoryPropertyFlags property_flags) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(gpu_, &memory_properties);
  for (uint32_t i(0); i < memory_properties.memoryTypeCount; ++i) {
    if (memory_type_bits & (1 << i) &&
        (memory_properties.memoryTypes[i].propertyFlags & property_flags) ==
            property_flags) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type");
}

SwapChainSupportDetails VulkanCore::GetSwapChainSupport() const {
  return SwapChainSupport(gpu_);
}

void VulkanCore::CreateInstance() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Vulkan app";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.pEngineName = "Slash";
  app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.apiVersion = VK_API_VERSION_1_1;

  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensionsProps(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensionsProps.data());

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  auto extensions = GetRequaredExtentions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

#if defined(SL_RENDERER_VALIDATION)
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (!CheckValidationLayers()) {
    throw std::runtime_error("validation layers requested? but not available");
  }
  create_info.enabledLayerCount =
      static_cast<uint32_t>(validation_layers.size());
  create_info.ppEnabledLayerNames = validation_layers.data();
  SetUpDebugInfo(debug_create_info);
  create_info.pNext = &debug_create_info;
#else
  create_info.enabledLayerCount = 0;
#endif

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan instance");
  }
}

void VulkanCore::CreateSurface() {
  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create widow surface");
  }
}

void VulkanCore::PickPhysicalDevice() {
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
      gpu_ = device;
      break;
    }
  }
  if (gpu_ == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable GPU");
  }
}

void VulkanCore::CreateLogicalDevice() {
  std::vector<VkDeviceQueueCreateInfo> queue_createinfos;
  auto queue_families = FindQueueFamilies(gpu_);
  std::set<uint32_t> unique_queue_families = {
      queue_families.graphicsFamily.value(),
      queue_families.presentFamily.value()};

  float queue_priority = 1.0f; // must be between 0 and 1
  for ([[maybe_unused]] uint32_t queueFamily : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_families.graphicsFamily.value();
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
      static_cast<uint32_t>(device_extentions.size());
  create_info.ppEnabledExtensionNames = device_extentions.data();

#if defined(SL_RENDERER_VALIDATION)
  create_info.enabledLayerCount =
      static_cast<uint32_t>(validation_layers.size());
  create_info.ppEnabledLayerNames = validation_layers.data();
#else
  create_info.enabledLayerCount = 0;
#endif

  if (vkCreateDevice(gpu_, &create_info, nullptr, &device_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device");
  }
}

std::vector<const char *> VulkanCore::GetRequaredExtentions() {
  uint32_t extensions_count = 0;
  const char **extensions;
  extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
  std::vector<const char *> return_extensions(extensions,
                                              extensions + extensions_count);
#if defined(SL_RENDERER_VALIDATION)
  return_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
  return return_extensions;
}

bool VulkanCore::IsDeviceSuitable(VkPhysicalDevice const &device) {
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

QueueFamilyIndices
VulkanCore::FindQueueFamilies(VkPhysicalDevice const &device) const {
  QueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queueFamilies.data());

  // Pick needed queue family
  uint32_t i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 &&
        queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);

    if (queueFamily.queueCount > 0 && present_support) {
      indices.presentFamily = i;
    }
    if (indices.IsComplete()) {
      break;
    }
    ++i;
  }
  return indices;
}

bool VulkanCore::CheckDeviceExtensionSupport(VkPhysicalDevice const &device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       available_extensions.data());

  std::set<std::string_view> required_extensions(device_extentions.begin(),
                                                 device_extentions.end());
  for (const auto &extention : available_extensions) {
    required_extensions.erase(extention.extensionName);
  }
  return required_extensions.empty();
}

SwapChainSupportDetails
VulkanCore::SwapChainSupport(VkPhysicalDevice const &device) const {
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

#if defined(SL_RENDERER_VALIDATION)

bool VulkanCore::CheckValidationLayers() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> availableLayers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());
  for (const auto &valLayer : validation_layers) {
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

void VulkanCore::SetUpDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
  SetUpDebugInfo(debug_info);
  if (CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr,
                                   &debug_utils_messenger_ext_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan Debug Messenger");
  }
}

VkBool32 VulkanCore::DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallBackData,
    void *pUserData) {
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

VkResult VulkanCore::CreateDebugUtilsMessengerEXT(
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

void VulkanCore::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

void VulkanCore::SetUpDebugInfo(VkDebugUtilsMessengerCreateInfoEXT &info) {
  info = {};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      //                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
      //                                     |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = DebugCallBack;
  info.pUserData = nullptr;
}

#endif

} // namespace slash