#include "vulkan_semaphore.hpp"

namespace slash {

VulkanSemaphore::VulkanSemaphore(VulkanCore *vcore) : vcore_(vcore) {
  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  if (vkCreateSemaphore(vcore_->GetDevice(), &semaphore_info, nullptr,
                        &semaphore_) != VK_SUCCESS)
    throw std::runtime_error("failed to create semaphore");
}

VulkanSemaphore::~VulkanSemaphore() {
  vkDestroySemaphore(vcore_->GetDevice(), semaphore_, nullptr);
}

VkSemaphore VulkanSemaphore::GetSemaphore() const { return semaphore_; }

} // namespace slash