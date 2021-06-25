#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_CORE_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_CORE_HPP_

#include "slash_pch.hpp"
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

  [[nodiscard]] bool complete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class VulkanCore {
 public:
  explicit VulkanCore(GLFWwindow *window);
  ~VulkanCore();

  [[nodiscard]] auto get_device() const -> const VkDevice &;
  [[nodiscard]] auto get_surface() const -> const VkSurfaceKHR &;
  [[nodiscard]] auto get_physical_device() const -> const VkPhysicalDevice &;
  [[nodiscard]] auto get_swap_chain_support() const -> SwapChainSupportDetails;
  [[nodiscard]] auto get_queue_families() const -> QueueFamilyIndices;
  [[nodiscard]] auto get_window_size() const -> std::pair<uint32_t, uint32_t>;
  [[nodiscard]] auto find_supported_format(const std::vector<VkFormat> &candidates,
                                           VkImageTiling tiling,
                                           VkFormatFeatureFlags features) -> VkFormat;
  [[nodiscard]] auto find_memory_type_index(uint32_t memory_type_bits,
                                            VkMemoryPropertyFlags property_flags) -> uint32_t;

 private:
  void create_instance();
  void create_surface();
  void pick_physical_device();
  void create_logical_device();

  [[nodiscard]] bool is_device_suitable(const VkPhysicalDevice &device);
  [[nodiscard]] auto find_queue_families(const VkPhysicalDevice &device) const -> QueueFamilyIndices;
  [[nodiscard]] bool check_device_extension_support(const VkPhysicalDevice &device);
  [[nodiscard]] auto swap_chain_support(const VkPhysicalDevice &device) const -> SwapChainSupportDetails;
  [[nodiscard]] auto get_required_extensions() -> std::vector<const char *>;

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
                                VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);
  const std::vector<const char *> validation_layers = {
      "VK_LAYER_KHRONOS_validation",
  };

  VkDebugUtilsMessengerEXT debug_utils_messenger_ext_;
#endif

 private:
  const std::vector<const char *> device_extensions = {
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
