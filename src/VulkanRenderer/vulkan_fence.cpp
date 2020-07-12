#include "vulkan_fence.hpp"

namespace slash {

VulkanFence::VulkanFence(VulkanCore *vcore) : vcore_(vcore) {
  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  if (vkCreateFence(vcore_->GetDevice(), &fence_info, nullptr, &fence_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create fence");
  }
}

VulkanFence::~VulkanFence() {
  vkDestroyFence(vcore_->GetDevice(), fence_, nullptr);
}
const VkFence & VulkanFence::GetFence() const { return fence_; }

} // namespace slash