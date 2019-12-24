#ifndef SL_VULKANRENDERER
#define SL_VULKANRENDERER

#include "SlashPCH.hpp"
#include "Core/Core.hpp"
#include "Renderer/RendererAPI.hpp"

#define GLFW_INCLUDE_VULKAN
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

        void Init() override;
        void Destroy() override;
    private:
        void CreateInstance();
        void SetUpDebugMessanger();
        void CreateSurface();
        bool CheckValidationLayers();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();

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

    private:
        VkInstance               instance;
        VkSurfaceKHR             surface;
        VkDebugUtilsMessengerEXT debugMessanger;
        VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE;
        VkDevice                 device;                          // logical device
        VkQueue                  graphicsQueue;
        VkQueue                  presentQueue;
        VkSwapchainKHR           swapChain;
        VkFormat                 swapChainFormat;
        VkExtent2D               swapChainExtent;
        std::vector<VkImage>     swapChainImages;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };

        const std::vector<const char*> deviceExtentions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
    };

} // namespace Slash

#endif