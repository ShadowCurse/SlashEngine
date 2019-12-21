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
            QueueFamilyIndices() = default;
            QueueFamilyIndices(QueueFamilyIndices& other)
            {
                graphicsFamily = other.graphicsFamily;
            }
            std::optional<uint32_t> graphicsFamily;

            bool IsComplete()
            {
                return graphicsFamily.has_value();
            }
        };
    
    public:
        VulkanRendererAPI() = default;
        ~VulkanRendererAPI();

        void Init() override;
        void Destroy() override;
    private:
        void CreateInstance();
        void SetUpDebugMessanger();
        bool CheckValidationLayers();
        void PickPhysicalDevice();
        void CreateLogicalDevice();

        std::vector<const char*> GetRequaredExtentions();

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
            void* pUserData);

        void SetUpDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator);

        bool IsDeviceSutable(const VkPhysicalDevice& device);
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);

    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessanger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsQueue;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };
    };

} // namespace Slash

#endif