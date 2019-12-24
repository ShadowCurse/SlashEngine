#include "VulkanRenderer/VulkanRendererAPI.hpp"
#include "../Core/Application.hpp"
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
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
    }
    
    void VulkanRendererAPI::Destroy()
    {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);
        DestroyDebugUtilsMessengerEXT(instance, debugMessanger, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
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

    void VulkanRendererAPI::CreateSurface()
    {
        if (glfwCreateWindowSurface(instance,
                                    static_cast<GLFWwindow*>(Slash::Application::Get()
                                    .GetWindow()
                                    .GetNativeWindow()),
                                    nullptr,
                                    &surface) != VK_SUCCESS){
            throw std::runtime_error("failde to create widow surface");
        }
    }

    // Get physical GPU in the system
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
            // TODO make some GPU score to pick one with most preformanse
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

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueFamilyFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f; // must be between 0 and 1
        for (uint32_t queueFamily: uniqueFamilyFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {}; // features that will be in use

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtentions.size());
        createInfo.ppEnabledExtensionNames = deviceExtentions.data();
        
        // if (enableValidationLayers)
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }
        
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(),  0, &presentQueue);
    }

    void VulkanRendererAPI::CreateSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (imageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // also can be VK_IMAGE_USAGE_TRANSFER_DST_BIT

        QueueFamilyIndices indicies = FindQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indicies.graphicsFamily.value(),
                                          indicies.presentFamily.value() };
        if (indicies.graphicsFamily != indicies.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Images can be used across multiple queue families without explicit ownership transfers.
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // An image is owned by one queue family at a time and ownership must be explicitly transfered before using it in another queue family. This option offers the best performance.
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        swapChainFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

//////////////////////////////////////////////////////
/////////// ADITIONAL FUNC ///////////////////////////
//////////////////////////////////////////////////////

    std::vector<const char*> VulkanRendererAPI::GetRequaredExtentions()
    {
        uint32_t extensionsCount = 0;
        const char** extensions;
        extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

        std::vector<const char*> retExtentions(extensions, extensions + extensionsCount);

        retExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return retExtentions;
    }

    // Debug callback for logging vulkan messages
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererAPI::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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
            SL_CORE_WARN("For objects: ");
            for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
                SL_CORE_WARN(pCallBackData->pObjects[i].pObjectName);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            SL_CORE_ERROR(pCallBackData->pMessage);
            SL_CORE_ERROR("For objects: ");
            for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
                SL_CORE_ERROR(pCallBackData->pObjects[i].pObjectName);
            break;
        }
        return VK_FALSE;
    }

    // Confiugures types of messages that will be recived
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
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanRendererAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator)
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    // Checks physical device if it has needed properties, features and queue families
    bool VulkanRendererAPI::IsDeviceSutable(const VkPhysicalDevice& device)
    {
        // TODO make props and features be choosable
        // Getting device properties
        // VkPhysicalDeviceProperties deviceProperties;
        // vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // // Getting device features
        // VkPhysicalDeviceFeatures deviceFeatures;
        // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Find apropriet queue families for device (specified in queue family property)
        QueueFamilyIndices indices = FindQueueFamilies(device);
        bool extentionsSupported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extentionsSupported) {
            auto swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() &&
                                !swapChainSupport.presentModes.empty();
        }

        // TODO make this configurable
        return indices.IsComplete() && extentionsSupported && swapChainAdequate;
    }

    // Gets queue family that has particular properties
    VulkanRendererAPI::QueueFamilyIndices VulkanRendererAPI::FindQueueFamilies(const VkPhysicalDevice& device)
    {
        QueueFamilyIndices indices;

        // Get count fo queue families
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        // Get queues properties
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // Pick needed queue family
        int i = 0;
        for (const auto& queueFamily: queueFamilies) 
        {
            // TODO make this configurable
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupprt = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupprt);

            if (queueFamily.queueCount > 0 && presentSupprt) {
                indices.presentFamily = i;
            }
            if (indices.IsComplete()) {
                break;
            }
            ++i;
        }
        return indices;
    }

    bool VulkanRendererAPI::checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
        uint32_t extentionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtentions(extentionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount, availableExtentions.data());

        std::set<std::string> requiredExtentions(deviceExtentions.begin(), deviceExtentions.end());

        for (const auto& extention: availableExtentions) {
            requiredExtentions.erase(extention.extensionName);
        }

        return requiredExtentions.empty();
    }

    VulkanRendererAPI::SwapChainSupportDetails VulkanRendererAPI::querySwapChainSupport(const VkPhysicalDevice& device) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());        
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());        
        }
        return details;
    }


    VkSurfaceFormatKHR VulkanRendererAPI::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat: availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return availableFormat;
                }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanRendererAPI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode: availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return availablePresentMode;
                }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRendererAPI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {Slash::Application::Get().GetWindow().GetWidth(),
                                       Slash::Application::Get().GetWindow().GetHeight()};
            actualExtent.width = std::max(capabilities.minImageExtent.width,
                                          std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.width = std::max(capabilities.minImageExtent.height,
                                          std::min(capabilities.maxImageExtent.height, actualExtent.height));
            return actualExtent;
        }
    }

    // Checks availability of validation layers
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
