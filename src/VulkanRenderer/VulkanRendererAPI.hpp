#ifndef SL_VULKANRENDERER
#define SL_VULKANRENDERER

#include "SlashPCH.hpp"
#include <GLFW/glfw3.h> 
#include "Core/Core.hpp"
#include "Core/Window.hpp"
#include "Renderer/RendererAPI.hpp"
#include "Renderer/Vertex.hpp"
#include "VulkanVertex.hpp"

namespace Slash
{

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    class VulkanRendererAPI : public RendererAPI
    {
    public:
        struct Config {
            struct InstanceConfig {
                std::string applicationName    = "Vulkan app";
                uint32_t    applicationVersion = VK_MAKE_VERSION(0, 0, 1);
                std::string engineName         = "Slash";
                uint32_t    engineVersion      = VK_MAKE_VERSION(0, 0, 1);
            };
            struct QueueFamilyConfig {
                uint32_t     minQueueCount = 0;
                VkQueueFlags queueFlags    = VK_QUEUE_GRAPHICS_BIT;
            };
            struct SwapChainConfig {
                VkFormat         format      = VK_FORMAT_B8G8R8A8_UNORM;
                VkColorSpaceKHR  colorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            };
            InstanceConfig instanceConfig;
            QueueFamilyConfig queueFalimyConfig;
            SwapChainConfig   swapChainConfig;

            VkPhysicalDeviceFeatures deviceFeatures = {};

            bool enableValidationLayers = true;
            std::vector<const char*> validationLayers = {
                "VK_LAYER_KHRONOS_validation",
            };
            std::vector<const char*> deviceExtentions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            };
        };
    private:
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool IsComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        struct SwapChainSupportDetails
        {
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

        Config& GetConfig() { return m_config; }
    private:
        void CreateInstance();
        void SetUpDebugMessanger();
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
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        void CreateCommandBuffers();
        void CreateSemaphores();
        void CreateFences();

        std::vector<const char*> GetRequaredExtentions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
                                                            void* pUserData);
        void SetUpDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks* pAllocator,
                                              VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                           const VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks* pAllocator);
        bool IsDeviceSutable(const VkPhysicalDevice& device);
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
        bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);

        SwapChainSupportDetails SwapChainSupport(const VkPhysicalDevice& device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        static std::vector<char> readShader(const std::string& filename);
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyByffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void UpdateUniformBuffer(float time, uint32_t currentImage);

    private:
        VkInstance                   instance;
        VkSurfaceKHR                 surface;
        VkDebugUtilsMessengerEXT     debugMessanger;
        VkPhysicalDevice             physicalDevice = VK_NULL_HANDLE;
        VkDevice                     device;
        VkQueue                      graphicsQueue;
        VkQueue                      presentQueue;
        VkSwapchainKHR               swapChain;
        VkFormat                     swapChainFormat;
        VkExtent2D                   swapChainExtent;
        std::vector<VkImage>         swapChainImages;
        std::vector<VkImageView>     swapChainImageViews;
        VkRenderPass                 renderPass;
        VkDescriptorSetLayout        descriptorSetLayout;
        VkPipelineLayout             pipelineLayout;
        VkPipeline                   graphicsPipeline;
        std::vector<VkFramebuffer>   swapChainFramebuffers;
        VkCommandPool                commandPool;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore>     imageAvailableSemaphores;
        std::vector<VkSemaphore>     renderFinishedSemaphores;
        std::vector<VkFence>         inFlightFences;
        std::vector<VkFence>         imagesInFlight;

        std::shared_ptr<Window> m_window;
        Window::WindowData* m_windowData;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        Config m_config = {};

        QueueFamilyIndices m_queueFamilyIndices = {};

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };

        const std::vector<const char*> deviceExtentions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        const size_t MAX_FRAMES_IN_FLIGHT = 2;
        size_t current_frame = 0;
    };

} // namespace Slash

#endif