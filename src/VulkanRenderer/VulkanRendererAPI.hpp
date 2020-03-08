#ifndef SLASHENGINE_VULKANRENDERER_VULKANRENDERER_H_
#define SLASHENGINE_VULKANRENDERER_VULKANRENDERER_H_

#include "slash_pch.hpp"
#include "Core/core.hpp"
#include "Core/window.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/renderer_api.hpp"
#include "Renderer/vertex.hpp"
#include "VulkanVertex.hpp"

namespace slash {

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

class VulkanRendererAPI : public RendererAPI {
public:
  struct Config {
    struct InstanceConfig {
      std::string applicationName = "Vulkan app";
      uint32_t applicationVersion = VK_MAKE_VERSION(0, 0, 1);
      std::string engineName = "Slash";
      uint32_t engineVersion = VK_MAKE_VERSION(0, 0, 1);
    } instance_config_;
    struct QueueFamilyConfig {
      uint32_t minQueueCount = 0;
      VkQueueFlags queueFlags = VK_QUEUE_GRAPHICS_BIT;
    } queue_family_config_;
    struct SwapChainConfig {
      VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
      VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    } swap_chain_config_;
    //            VkPhysicalDeviceFeatures deviceFeatures = {};

    bool enableValidationLayers = true;
    std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };
    std::vector<const char *> deviceExtentions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
  };

private:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

public:
  VulkanRendererAPI() = default;
  ~VulkanRendererAPI() final = default;
  void AddWindow(std::shared_ptr<Window> window) final;
  void Init() final;
  void Destroy() final;
  void DrawFrame(float time) final;

  Config &GetConfig() { return config_; }

private:
  void CreateInstance();
  void SetUpDebugMessenger();
  void GetWindowData();
  void CreateSurface();
  bool CheckValidationLayers();
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateSwapChain();
  void DestroySwapChain();
  void RecreateSwapChain();
  void CreateImageViews();
  void CreateRenderPass();
  void CreateDescriptorSetLayout();
  void CreateGraphicsPipeline();
  void CreateFramebuffers();
  void CreateCommandPool();
  void CreateDepthResources();
  void CreateTextureImage();
  void CreateTextureImageView();
  void CreateTextureSampler();
  void CreateVertexBuffer();
  void CreateIndexBuffer();
  void CreateUniformBuffers();
  void CreateDescriptorPool();
  void CreateDescriptorSets();
  void CreateCommandBuffers();
  void CreateSemaphores();
  void CreateFences();

  std::vector<const char *> GetRequaredExtentions();
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallBackData,
                void *pUserData);
  void
  SetUpDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);
  void
  DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                const VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);
  bool IsDeviceSutable(const VkPhysicalDevice &device);
  QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &device);
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device);

  SwapChainSupportDetails SwapChainSupport(const VkPhysicalDevice &device);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  static std::vector<char> readShader(const std::string &filename);
  VkShaderModule CreateShaderModule(const std::vector<char> &code);

  uint32_t FindMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  void UpdateUniformBuffer(float time, uint32_t currentImage);

  void CreateImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory);

  VkCommandBuffer BeginSingleTimeCommand();
  void EndSingleTimeCommand(VkCommandBuffer command_buffer);

  void TransitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);
  void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  VkImageView CreateImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags);

  VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
  VkFormat FindDepthFormat();

private:
  VkInstance instance_;
  VkSurfaceKHR surface_khr_;
  VkDebugUtilsMessengerEXT debug_utils_messenger_ext_;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_;
  VkQueue graphics_queue_;
  VkQueue present_queue_;
  VkSwapchainKHR swap_chain_;
  VkFormat swap_chain_format_;
  VkExtent2D swap_chain_extent_;
  std::vector<VkImage> swap_chain_images_;
  std::vector<VkImageView> swap_chain_image_views_;
  VkRenderPass render_pass_;
  VkDescriptorSetLayout descriptor_set_layout_;
  VkPipelineLayout pipeline_layout_;
  VkPipeline graphics_pipeline_;
  std::vector<VkFramebuffer> swap_chain_framebuffers_;
  VkCommandPool command_pool_;
  std::vector<VkCommandBuffer> command_buffers_;
  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> inflight_fences_;
  std::vector<VkFence> images_inflight_;
  std::shared_ptr<Window> window_;
  Window::WindowData *window_data_;
  VkBuffer vertex_buffer_;
  VkDeviceMemory vertex_buffer_memory_;
  VkBuffer index_buffer_;
  VkDeviceMemory index_buffer_memory_;
  std::vector<VkBuffer> uniform_buffers_;
  std::vector<VkDeviceMemory> uniform_buffers_memory_;
  VkDescriptorPool descriptor_pool_;
  std::vector<VkDescriptorSet> descriptor_sets_;
  VkImage texture_image_;
  VkDeviceMemory texture_image_memory_;
  VkImageView texture_image_view_;
  VkSampler texture_sampler_;
  VkImage depth_image_;
  VkDeviceMemory depth_image_memory_;
  VkImageView depth_image_view_;

  Config config_ = {};

  QueueFamilyIndices queue_family_indices_ = {};

  const std::vector<const char *> validation_layers_ = {
      "VK_LAYER_KHRONOS_validation",
  };

  const std::vector<const char *> device_extentions_ = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  const size_t MAX_FRAMES_IN_FLIGHT = 2;
  size_t current_frame = 0;
};

} // namespace slash

#endif // SLASHENGINE_VULKANRENDERER_VULKANRENDERER_H_