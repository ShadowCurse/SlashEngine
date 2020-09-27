#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKANRENDERER_H_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKANRENDERER_H_

#include "Core/core.hpp"
#include "Core/window.hpp"
#include "GLFW/glfw3.h"
#include "Scene/camera.hpp"
//#include "Renderer/renderer_api.hpp"
#include "Renderer/vertex.hpp"
#include "vulkan_imgui.hpp"
#include "vulkan_vertex.hpp"

namespace slash {

class Slash_API VulkanRendererAPI final : public RendererAPI {
public:
  VulkanRendererAPI(Window *window) {}
  void AddWindow(std::shared_ptr<Window> window) final;
  void Init() final;
  void Destroy() final;
  void DrawFrame(double time) final;
  void UpdateScene() final;

  void BindModel(std::shared_ptr<Model> model) final;
  void UnBindModel(std::shared_ptr<Model> model) final;
  void BindMesh(std::shared_ptr<Mesh_3D> mesh) final;
  void UnBindMesh(std::shared_ptr<Mesh_3D> mesh) final;
  void BindTexture(std::shared_ptr<Texture> texture) final;
  void UnBindTexture(std::shared_ptr<Texture> texture) final;

private:
  struct Config {
    struct InstanceConfig {
      std::string application_name = "Vulkan app";
      uint32_t application_version = VK_MAKE_VERSION(0, 0, 1);
      std::string engineName = "Slash";
      uint32_t engine_version = VK_MAKE_VERSION(0, 0, 1);
    } instance_config;
    struct QueueFamilyConfig {
      uint32_t min_queue_count = 0;
      VkQueueFlags queue_flags = VK_QUEUE_GRAPHICS_BIT;
    } queue_family_config;
    struct SwapChainConfig {
      VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
      VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    } swap_chain_config;
    //            VkPhysicalDeviceFeatures deviceFeatures = {};

    bool enable_validation_layers = true;
    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    const std::vector<const char *> device_extentions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const size_t k_max_frames_in_flight = 2;
  } config_;

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

  // set up
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
  void CreateTextureSampler();
  void CreateCommandBuffers();
  void UpdateCommandBuffers();
  void CreateSemaphores();
  void CreateFences();

  // main

  void CreateCameraBuffer();
  void DestroyCameraBuffer();

  void AddRotationBuffer(size_t uid);
  void DestroyRotationBuffer(size_t uid);

  void CreateDescriptorPool(uint32_t size);
  void DestroyDescriptorPool();
  void CreateDescriptorSet(size_t object_uid, size_t mesh_uid,
                           size_t texture_uid);
  void DestroyDescriptorSet(size_t object_uid);

  // additional
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
  bool IsDeviceSuitable(const VkPhysicalDevice &device);
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

  void UpdateUniformBuffer(double time, uint32_t currentImage);

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

  VkInstance instance_;
  VkSurfaceKHR surface_;
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

  VulkanImGui imgui_;

  Window::WindowData *window_data_;
  Camera camera_;

  std::vector<std::shared_ptr<Model>> models_;

  VkDescriptorSetLayout descriptor_set_layout_;
  VkDescriptorPool descriptor_pool_;
  std::vector<std::pair<bool, std::vector<VkDescriptorSet>>> descriptor_sets_;
  std::map<size_t, std::pair<bool, std::vector<VkDescriptorSet>> *>
      model_descriptor_sets_;

  VkBuffer camera_buffer_; // for camera
  VkDeviceMemory camera_buffer_memory_;

  std::map<size_t, std::vector<VkBuffer>>
      rotation_buffers_; // for model rotations
  std::map<size_t, std::vector<VkDeviceMemory>> rotation_buffers_memory_;

  std::map<size_t, VkBuffer> vertex_buffers_;
  std::map<size_t, VkDeviceMemory> vertex_buffers_memory_;
  std::map<size_t, VkDeviceSize> vertex_offsets_;
  std::map<size_t, VkBuffer> index_buffers_;
  std::map<size_t, VkDeviceMemory> index_buffers_memory_;
  std::map<size_t, uint32_t> index_buffers_size_;

  std::map<size_t, VkImage> texture_images_;
  std::map<size_t, VkDeviceMemory> texture_images_memory_;
  std::map<size_t, VkImageView> texture_images_view_;
  VkSampler texture_sampler_;

  VkImage depth_image_;
  VkDeviceMemory depth_image_memory_;
  VkImageView depth_image_view_;

  QueueFamilyIndices queue_family_indices_ = {};
  size_t current_frame = 0;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKANRENDERER_H_