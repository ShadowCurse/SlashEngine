#include "vulkan_vertex_buffer.hpp"

namespace slash {

VulkanVertexBuffer::VulkanVertexBuffer(VulkanCore *vcore, VkDeviceSize size,
                                       uint32_t offset)
    : VulkanBuffer(vcore, size,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      offset_(offset) {}

} // namespace slash
