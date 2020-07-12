#include "vulkan_command_pool.hpp"

slash::VulkanCommandPool::VulkanCommandPool(VulkanCore *vcore,
                                            uint32_t queue_family_index,
                                            VkCommandPoolCreateFlags flags)
    : vcore_(vcore), queue_family_index_(queue_family_index), flags_(flags) {
  VkCommandPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.queueFamilyIndex = queue_family_index_;
  create_info.flags = flags_;
  create_info.pNext = nullptr;
  if (vkCreateCommandPool(vcore_->GetDevice(), &create_info, nullptr, &pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool");
  }
}
slash::VulkanCommandPool::~VulkanCommandPool() {
  vkDestroyCommandPool(vcore_->GetDevice(), pool_, nullptr);
}

const VkCommandPool &slash::VulkanCommandPool::GetPool() const { return pool_; }
