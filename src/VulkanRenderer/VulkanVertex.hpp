#ifndef SL_VULKANVERTEX
#define SL_VULKANVERTEX

#include "GLFW/glfw3.h"
#include "Renderer/Vertex.hpp"

namespace Slash {

    struct VulkanVertex
    {
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptionds() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDesctiption = {};

            attributeDesctiption[0].binding = 0;
            attributeDesctiption[0].location = 0;
            attributeDesctiption[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDesctiption[0].offset = offsetof(Vertex, pos);

            attributeDesctiption[1].binding = 0;
            attributeDesctiption[1].location = 1;
            attributeDesctiption[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDesctiption[1].offset = offsetof(Vertex, color);

            return attributeDesctiption;
        }
    };
} // namespace Slash

#endif