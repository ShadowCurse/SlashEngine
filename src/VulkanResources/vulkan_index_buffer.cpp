#include "vulkan_index_buffer.hpp"

namespace slash {

VulkanIndexBuffer::VulkanIndexBuffer(VulkanCore *vcore,
                                     VkDeviceSize buffer_size,
                                     uint32_t elements_num)
    : VulkanBuffer(vcore, buffer_size,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      elements_num_(elements_num) {}

uint32_t VulkanIndexBuffer::GetElementsNum() const { return elements_num_; }

} // namespace slash