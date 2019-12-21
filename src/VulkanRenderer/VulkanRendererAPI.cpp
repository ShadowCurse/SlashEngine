#include "VulkanRenderer/VulkanRendererAPI.hpp"
#include "Core/Log.hpp"

namespace Slash
{
    VulkanRendererAPI::~VulkanRendererAPI()
    {
    }

    void VulkanRendererAPI::Init()
    {
        CreateInstance();
        SetUpDebugMessanger();
        PickPhysicalDevice();
    }
    
    void VulkanRendererAPI::Destroy()
    {
        vkDestroyDevice(device, nullptr);
        DestroyDebugUtilsMessengerEXT(instance, debugMessanger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void VulkanRendererAPI::CreateInstance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan app";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.pEngineName = "Slash";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionsProps(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProps.data());

        SL_CORE_INFO("Available extentions: ");
        for (const auto& ext: extensionsProps) {
            SL_CORE_INFO(ext.extensionName);
        }

        auto extensions = GetRequaredExtentions();
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (CheckValidationLayers()) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            SetUpDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan instance");
        }
    }

    void VulkanRendererAPI::SetUpDebugMessanger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        SetUpDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessanger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan Debug Messenger");
        }
    }

    void VulkanRendererAPI::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        
        if (deviceCount == 0) {
            throw std::runtime_error("Failed to fing GPUs with Vulkan supporn");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device: devices) {
            if (IsDeviceSutable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a sutable GPU");
        }
    }

    void VulkanRendererAPI::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

        VkDeviceQueueCreateInfo queueCreateinfo = {};
        queueCreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateinfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateinfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateinfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateinfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        
        // if (enableValidationLayers)
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }
        
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    }

    std::vector<const char*> VulkanRendererAPI::GetRequaredExtentions()
    {
        uint32_t extensionsCount = 0;
        const char** extensions;
        extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

        std::vector<const char*> retExtentions(extensions, extensions + extensionsCount);

        retExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return retExtentions;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererAPI::DebugCallBack(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
        void* pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            SL_CORE_TRACE(pCallBackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            SL_CORE_INFO(pCallBackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            SL_CORE_WARN(pCallBackData->pMessage);
            // SL_CORE_WARN("For objects: ");
            // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
                // SL_CORE_WARN(pCallBackData->pObjects + i);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            SL_CORE_ERROR(pCallBackData->pMessage);
            // SL_CORE_ERROR("For objects: ");
            // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
                // SL_CORE_ERROR(pCallBackData->pObjects[i]);
            break;
        }
        return VK_FALSE;
    }

    void VulkanRendererAPI::SetUpDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallBack;
        createInfo.pUserData = nullptr;
    }

    VkResult VulkanRendererAPI::CreateDebugUtilsMessengerEXT(VkInstance instance,
                            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                            const VkAllocationCallbacks* pAllocator, 
                            VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanRendererAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }   
    }

    bool VulkanRendererAPI::IsDeviceSutable(const VkPhysicalDevice& device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = FindQueueFamilies(device);

        return indices.IsComplete() && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    }

    VulkanRendererAPI::QueueFamilyIndices VulkanRendererAPI::FindQueueFamilies(const VkPhysicalDevice& device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily: queueFamilies) 
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            ++i;
        }
        return indices;
    }

    bool VulkanRendererAPI::CheckValidationLayers()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const auto& valLayer: validationLayers)
        {
            bool layersFound = false;
            for (const auto& layer: availableLayers)
            {
                if (strcmp(valLayer, layer.layerName) == 0) {
                    layersFound = true;
                    break;
                }
            }

            if (!layersFound)
                return false;
        }
        return true;
    }

} // namespace Slash
