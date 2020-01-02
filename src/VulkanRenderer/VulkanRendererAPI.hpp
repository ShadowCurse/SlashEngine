#ifndef SL_VULKANRENDERER
#define SL_VULKANRENDERER

#include "SlashPCH.hpp"
#include "Core/Core.hpp"
#include "Renderer/RendererAPI.hpp"
#include <GLFW/glfw3.h>

namespace Slash
{
    class VulkanRendererAPI : public RendererAPI
    {
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
        ~VulkanRendererAPI();

        void Init() final;
        void Destroy() final;
        void DrawFrame() final;
    private:
        void CreateInstance();
        void SetUpDebugMessanger();
        void CreateSurface();
        bool CheckValidationLayers();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
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
        bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);

        SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D         chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        static std::vector<char> readShader(const std::string& filename);
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

    private:
        VkInstance                   instance;
        VkSurfaceKHR                 surface;
        VkDebugUtilsMessengerEXT     debugMessanger;
        VkPhysicalDevice             physicalDevice = VK_NULL_HANDLE;
        VkDevice                     device;                          // logical device
        VkQueue                      graphicsQueue;
        VkQueue                      presentQueue;
        VkSwapchainKHR               swapChain;
        VkFormat                     swapChainFormat;
        VkExtent2D                   swapChainExtent;
        std::vector<VkImage>         swapChainImages;
        std::vector<VkImageView>     swapChainImageViews;
        VkRenderPass                 renderPass;
        VkPipelineLayout             pipelineLayout;
        VkPipeline                   graphicsPipeline;
        std::vector<VkFramebuffer>   swapChainFramebuffers;
        VkCommandPool                commandPool;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore>     imageAvailableSemaphores;
        std::vector<VkSemaphore>     renderFinishedSemaphores;
        std::vector<VkFence>         inFlightFences;
        std::vector<VkFence>         imagesInFlight;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };

        const std::vector<const char*> deviceExtentions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        const int MAX_FRAMES_IN_FLIGHT = 2;
        size_t current_frame = 0;
    };

} // namespace Slash

#endif