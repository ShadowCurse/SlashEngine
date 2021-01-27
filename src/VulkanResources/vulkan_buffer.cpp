#include "vulkan_buffer.hpp"

namespace slash {

VulkanBuffer::VulkanBuffer(VulkanCore *vcore, VkDeviceSize buffer_size,
                           VkBufferUsageFlags usage_flags,
                           VkMemoryPropertyFlags memory_property_flags)
    : vcore_(vcore), buffer_size_(buffer_size) {

  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = buffer_size;
  buffer_info.usage = usage_flags;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vcore_->get_device(), &buffer_info, nullptr, &buffer_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer");
  }

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(vcore_->get_device(), buffer_,
                                &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = vcore_->find_memory_type_index(
      mem_requirements.memoryTypeBits, memory_property_flags);
  if (vkAllocateMemory(vcore_->get_device(), &alloc_info, nullptr, &memory_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory");
  }
  vkBindBufferMemory(vcore_->get_device(), buffer_, memory_, 0);
}

VulkanBuffer::~VulkanBuffer() {
  vkDestroyBuffer(vcore_->get_device(), buffer_, nullptr);
  vkFreeMemory(vcore_->get_device(), memory_, nullptr);
}

VkBuffer VulkanBuffer::GetBuffer() const { return buffer_; }

VkDeviceMemory VulkanBuffer::GetMemory() const { return memory_; }

VkDeviceSize VulkanBuffer::GetBufferSize() const { return buffer_size_; }

VulkanStagingBuffer::VulkanStagingBuffer(VulkanCore *vcore,
                                         VkDeviceSize buffer_size)
    : VulkanBuffer(vcore, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}
} // namespace slash
