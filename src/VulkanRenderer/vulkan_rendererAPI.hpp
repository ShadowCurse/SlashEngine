#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKANRENDERER_H_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKANRENDERER_H_

#include "Core/core.hpp"
#include "Core/window.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/renderer_api.hpp"
#include "Renderer/vertex.hpp"
#include "vulkan_vertex.hpp"

namespace slash {

class VulkanBufferManager;

class Slash_API VulkanRendererAPI : public RendererAPI {
public:
  friend class VulkanBufferManager;

  VulkanRendererAPI() = default;
  ~VulkanRendererAPI() final = default;
  void AddWindow(std::shared_ptr<Window> window) final;
  void Init() final;
  void Destroy() final;
  void DrawFrame(float time) final;
  void UpdateScene() final;
  void BindVertexBuffer(size_t uid, const std::vector<Vertex> &vertices) final;
  void BindIndexBuffer(size_t uid, const std::vector<uint16_t> &indices) final;
  void UnBindVertexBuffer(size_t uid) final;
  void UnBindIndexBuffer(size_t uid) final;

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

  void CreateSubSystems();
  void DestroySubSystems();

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
  void CreateTextureImage(); // TODO
  void CreateTextureImageView();
  void CreateTextureSampler();
  void CreateVertexBuffer();
  void CreateIndexBuffer();
  void CreateUniformBuffers(); // TODO
  void CreateDescriptorPool();
  void CreateDescriptorSets(); // TODO
  void CreateCommandBuffers();
  void UpdateCommandBuffers();
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

  //  VkBuffer vertex_buffer_;
  //  VkDeviceMemory vertex_buffer_memory_;
  //  VkBuffer index_buffer_;
  //  VkDeviceMemory index_buffer_memory_;

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

  QueueFamilyIndices queue_family_indices_ = {};
  size_t current_frame = 0;

  Unique<VulkanBufferManager> buffer_manager_;
};

class Slash_API VulkanBufferManager {
public:
  friend class VulkanRendererAPI;
  explicit VulkanBufferManager(VulkanRendererAPI *renderer_api);
  ~VulkanBufferManager();

private:
  void AddVertexBuffer(size_t uid, const std::vector<Vertex> &vertices);
  void AddIndexBuffer(size_t uid, const std::vector<uint16_t> &indices);
  void RemoveVertexBuffer(size_t uid);
  void RemoveIndexBuffer(size_t uid);

  [[nodiscard]] bool HasData() const;
  [[nodiscard]] size_t Size() const;
  [[nodiscard]] const VkBuffer *GetVertexBuffer(size_t index) const;
  [[nodiscard]] const VkDeviceSize *GetVertexBuffersOffsets(size_t index) const;
  [[nodiscard]] VkBuffer GetIndexBuffer(size_t index) const;
  [[nodiscard]] uint32_t GetIndexBufferSize(size_t index) const;

  VulkanRendererAPI *renderer_api_;
  std::vector<size_t> buffer_uids_;
  std::map<size_t, VkBuffer> vertex_buffers_;
  std::map<size_t, VkDeviceMemory> vertex_buffers_memory_;
  std::map<size_t, VkDeviceSize> vertex_offsets_;

  std::map<size_t, VkBuffer> index_buffers_;
  std::map<size_t, VkDeviceMemory> index_buffers_memory_;
  std::map<size_t, uint32_t> index_buffers_size_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKANRENDERER_H_