#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_CORE_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_CORE_HPP_

#include "Core/core.hpp"
#include "GLFW/glfw3.h"

namespace slash {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class VulkanCore {
public:
  explicit VulkanCore(GLFWwindow *window);
  ~VulkanCore();

  [[nodiscard]] const VkDevice &GetDevice() const;
  [[nodiscard]] const VkSurfaceKHR& GetSuface() const;
  [[nodiscard]] const VkPhysicalDevice &GetPhysicalDevice() const;
  [[nodiscard]] SwapChainSupportDetails GetSwapChainSupport() const;
  [[nodiscard]] QueueFamilyIndices GetQueueFamilies() const;
  [[nodiscard]] std::pair<uint32_t, uint32_t> GetWindowSize() const;
  [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  [[nodiscard]] uint32_t FindMemoryTypeIndex(uint32_t memory_type_bits, VkMemoryPropertyFlags property_flags);

private:
  void CreateInstance();
  void CreateSurface();
  void PickPhysicalDevice();
  void CreateLogicalDevice();

  bool IsDeviceSuitable(const VkPhysicalDevice &device);
  QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &device) const;
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device);
  SwapChainSupportDetails SwapChainSupport(const VkPhysicalDevice &device) const;
  std::vector<const char *> GetRequaredExtentions();

#if defined(SL_RENDERER_VALIDATION)
  bool CheckValidationLayers();
  void SetUpDebugMessenger();
  void SetUpDebugInfo(VkDebugUtilsMessengerCreateInfoEXT &info);
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallBackData,
                void *pUserData);
  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);
  void
  DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                const VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);
  const std::vector<const char *> validation_layers = {
      "VK_LAYER_KHRONOS_validation",
  };

  VkDebugUtilsMessengerEXT debug_utils_messenger_ext_;
#endif

  const std::vector<const char *> device_extentions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  GLFWwindow *window_;
  VkSurfaceKHR surface_;

  VkInstance instance_;
  VkPhysicalDevice gpu_;
  VkDevice device_;
};
} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_CORE_HPP_
