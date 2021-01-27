#include "vulkan_queue.hpp"

namespace slash {

VulkanQueue::VulkanQueue(VulkanCore *vcore, uint32_t family_index)
    : vcore_(vcore), family_index_(family_index), queue_(VK_NULL_HANDLE) {
  vkGetDeviceQueue(vcore->get_device(), family_index, 0, &queue_);
}

uint32_t VulkanQueue::GetFamilyIndex() const { return family_index_; }

VkQueue VulkanQueue::GetQueue() const { return queue_; }

} // namespace slash