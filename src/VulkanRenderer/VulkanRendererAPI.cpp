#include "VulkanRenderer/VulkanRendererAPI.hpp"
#include "SlashPCH.hpp"
#include "Core/Application.hpp"
#include "Core/Log.hpp"

namespace Slash
{
    void  VulkanRendererAPI::AddWindow(std::shared_ptr<Window> window)
    {
        m_window = window;
    }

    void VulkanRendererAPI::Init()
    {
        CreateInstance();
        SetUpDebugMessanger();
        GetWindowData();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSemaphores();
        CreateFences();
    }
    
    void VulkanRendererAPI::Destroy()
    {
        vkDeviceWaitIdle(device);
        for (size_t i(0); i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
        DestroySwapChain();
        vkDestroyCommandPool(device, commandPool, nullptr);
        vkDestroyDevice(device, nullptr);
        if (m_config.enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(instance, debugMessanger, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void VulkanRendererAPI::DrawFrame()
    {
        vkWaitForFences(device, 1, &inFlightFences[current_frame], VK_TRUE, UINT64_MAX);
        uint32_t imageIndex;
        auto result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[current_frame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            m_windowData->resized = false;
            RecreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[imageIndex] = inFlightFences[current_frame];
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[current_frame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[current_frame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        vkResetFences(device, 1, &inFlightFences[current_frame]);
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer");
        }
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;
        result = vkQueuePresentKHR(presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_windowData->resized) {
            m_windowData->resized = false;
            RecreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
        current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

//////////////////////////////////////////////////////
/////////// PRIVATE FUNC /////////////////////////////
//////////////////////////////////////////////////////

    void VulkanRendererAPI::CreateInstance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan app";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.pEngineName = "Slash";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionsProps(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProps.data());

        SL_CORE_INFO("Available extentions: ");
        for (const auto& ext: extensionsProps) {
            SL_CORE_INFO(ext.extensionName);
        }

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequaredExtentions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (m_config.enableValidationLayers && !CheckValidationLayers()) {
            throw std::runtime_error("validation layers requested? but not available");
        }
        if (m_config.enableValidationLayers && CheckValidationLayers()) {
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
        VkDebugUtilsMessengerCreateInfoEXT debugInfo;
        SetUpDebugMessengerCreateInfo(debugInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessanger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan Debug Messenger");
        }
    }

    void VulkanRendererAPI::GetWindowData()
    {
        if (!m_window)
            throw std::runtime_error("no windows assigned to renderer");
        m_windowData = static_cast<Window::WindowData*>(glfwGetWindowUserPointer(static_cast<GLFWwindow*>(m_window->GetNativeWindow())));
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
        m_queueFamilyIndices = FindQueueFamilies(physicalDevice);
    }

    void VulkanRendererAPI::CreateLogicalDevice()
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { m_queueFamilyIndices.graphicsFamily.value(),
                                                   m_queueFamilyIndices.presentFamily.value() };

        float queuePriority = 1.0f; // must be between 0 and 1
        for ([[maybe_unused]] uint32_t queueFamily: uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // VkPhysicalDeviceFeatures deviceFeatures = {}; // features that will be in use

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &m_config.deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtentions.size());
        createInfo.ppEnabledExtensionNames = deviceExtentions.data();
        
        if (m_config.enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }
        
        vkGetDeviceQueue(device, m_queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, m_queueFamilyIndices.presentFamily.value(),  0, &presentQueue);
    }

    void VulkanRendererAPI::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = SwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR   presentMode   = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D         extent        = ChooseSwapExtent(swapChainSupport.capabilities);

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

        // QueueFamilyIndices indicies = FindQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { m_queueFamilyIndices.graphicsFamily.value(),
                                          m_queueFamilyIndices.presentFamily.value() };
        if (m_queueFamilyIndices.graphicsFamily != m_queueFamilyIndices.presentFamily) {
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

    void VulkanRendererAPI::DestroySwapChain()
    {
        for (size_t i(0); i < swapChainFramebuffers.size(); ++i)
            vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
        vkFreeCommandBuffers(device,
                             commandPool,
                             static_cast<uint32_t>(commandBuffers.size()),
                             commandBuffers.data());
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device,pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
        for (size_t i(0); i < swapChainImageViews.size(); ++i)
            vkDestroyImageView(device, swapChainImageViews[i], nullptr);
        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void VulkanRendererAPI::RecreateSwapChain()
    {
        vkDeviceWaitIdle(device);
        DestroySwapChain();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandBuffers();
    }

    void VulkanRendererAPI::CreateImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i(0); i < swapChainImages.size(); ++i) {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void VulkanRendererAPI::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapChainFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass");
        }
    }

    void VulkanRendererAPI::CreateGraphicsPipeline()
    {

        // reading code from files
        auto vertShaderCode = readShader("/home/antaraz/Projects/SlashEngine/src/Shaders/vert.spv");
        auto fragShaderCode = readShader("/home/antaraz/Projects/SlashEngine/src/Shaders/frag.spv");
        
        // creating modules from code
        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        // vertex stage info
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        vertShaderStageInfo.pSpecializationInfo = nullptr;

        // fragment stage info
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        fragShaderStageInfo.pSpecializationInfo = nullptr;

        // array of pipeline stage infos
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        // info of data that is passed to vertex shader
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        // type of geometry will be drawn
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0,0 };
        scissor.extent = swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                              VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT |
                                              VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        // VkDynamicState dynamicStates[] = {
        //     VK_DYNAMIC_STATE_VIEWPORT,
        //     VK_DYNAMIC_STATE_LINE_WIDTH,
        // };

        // VkPipelineDynamicStateCreateInfo dynamicState = {};
        // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        // dynamicState.dynamicStateCount = 2;
        // dynamicState.pDynamicStates = dynamicStates;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline");
        }

        vkDestroyShaderModule(device, vertShaderModule, nullptr);
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
    }

    void VulkanRendererAPI::CreateFramebuffers()
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i(0); i < swapChainImageViews.size(); ++i) {
            VkImageView attachments[] = {
                swapChainImageViews[i]
            };
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;
            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer");
            }
        }
    }

    void VulkanRendererAPI::CreateCommandPool()
    {
        // QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0;
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool");
        }
    }

    void VulkanRendererAPI::CreateCommandBuffers()
    {
        commandBuffers.resize(swapChainFramebuffers.size());
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command buffers");
        }

        for (size_t i(0); i < commandBuffers.size(); ++i) {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;
            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapChainExtent;
            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer");
            }
        }
    }
     
    void VulkanRendererAPI::CreateSemaphores()
    {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (size_t i(0); i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i])
                != VK_SUCCESS) {
                    throw std::runtime_error("failed to create semaphores for frame");
                }
        }
    }
    void VulkanRendererAPI::CreateFences()
    {
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE); 
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (size_t i(0); i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create fence");
            }
        }
    }

//////////////////////////////////////////////////////
/////////// ADITIONAL FUNC ///////////////////////////
//////////////////////////////////////////////////////

    // Get requared Vulkan instance extensions required by GLFW
    std::vector<const char*> VulkanRendererAPI::GetRequaredExtentions()
    {
        uint32_t extensionsCount = 0;
        const char** extensions;
        extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
        std::vector<const char*> retExtentions(extensions, extensions + extensionsCount);
        if (m_config.enableValidationLayers)
            retExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        return retExtentions;
    }

    // Debug callback for logging vulkan messages
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererAPI::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
                                                                    [[maybe_unused]] void* pUserData)
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
            //     SL_CORE_WARN(pCallBackData->pObjects[i].pObjectName);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            SL_CORE_ERROR(pCallBackData->pMessage);
            // SL_CORE_ERROR("For objects: ");
            // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
            //     SL_CORE_ERROR(pCallBackData->pObjects[i].pObjectName);
            break;
        default:
            SL_CORE_ERROR(pCallBackData->pMessage);
            // SL_CORE_ERROR("For objects: ");
            // for (uint32_t i(0); i < pCallBackData->objectCount; ++i)
            //     SL_CORE_ERROR(pCallBackData->pObjects[i].pObjectName);
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
        bool extentionsSupported = CheckDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extentionsSupported) {
            auto swapChainSupport = SwapChainSupport(device);
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

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // Pick needed queue family
        int i = 0;
        for (const auto& queueFamily: queueFamilies) 
        {
            if (queueFamily.queueCount > m_config.queueFalimyConfig.minQueueCount && 
                queueFamily.queueFlags & m_config.queueFalimyConfig.queueFlags ) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupprt = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupprt);

            if (queueFamily.queueCount > m_config.queueFalimyConfig.minQueueCount && presentSupprt) {
                indices.presentFamily = i;
            }
            if (indices.IsComplete()) {
                break;
            }
            ++i;
        }
        return indices;
    }

    bool VulkanRendererAPI::CheckDeviceExtensionSupport(const VkPhysicalDevice& device) 
    {
        uint32_t extentionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtentions(extentionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount, availableExtentions.data());

        std::set<std::string_view> requiredExtentions(m_config.deviceExtentions.begin(), m_config.deviceExtentions.end());
        for (const auto& extention: availableExtentions) {
            requiredExtentions.erase(extention.extensionName);
        }
        return requiredExtentions.empty();
    }  

    VulkanRendererAPI::SwapChainSupportDetails VulkanRendererAPI::SwapChainSupport(const VkPhysicalDevice& device)
    {
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


    VkSurfaceFormatKHR VulkanRendererAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat: availableFormats) {
            if (availableFormat.format ==  m_config.swapChainConfig.format &&
                availableFormat.colorSpace == m_config.swapChainConfig.colorSpace) {
                    return availableFormat;
                }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanRendererAPI::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode: availablePresentModes) {
            if (availablePresentMode == m_config.swapChainConfig.presentMode) {
                    return availablePresentMode;
                }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRendererAPI::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(static_cast<GLFWwindow*>(Slash::Application::Get().GetWindow().GetNativeWindow()),
                                   &width,
                                   &height);
            VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                       static_cast<uint32_t>(height)};
            actualExtent.width = std::max(capabilities.minImageExtent.width,
                                          std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.width = std::max(capabilities.minImageExtent.height,
                                          std::min(capabilities.maxImageExtent.height, actualExtent.height));
            return actualExtent;
        }
    }

    std::vector<char> VulkanRendererAPI::readShader(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file");
        }
        size_t size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(size);
        file.seekg(0);
        file.read(buffer.data(), size);
        return buffer;
    }

    VkShaderModule VulkanRendererAPI::CreateShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
        return shaderModule;
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
