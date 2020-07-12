#include "vulkan_descriptor_pool.hpp"

namespace slash {

VulkanDescriptorPool::VulkanDescriptorPool(
    VulkanCore *vcore, std::vector<VkDescriptorSetLayoutBinding> bindings,
    uint32_t size)
    : vcore_(vcore), max_sets_(size) {

  for (auto &binding : bindings) {
    pool_sizes_.emplace_back(VkDescriptorPoolSize{
        binding.descriptorType,
        static_cast<uint32_t>(binding.descriptorCount * size)});
  }
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes_.size());
  pool_info.pPoolSizes = pool_sizes_.data();
  pool_info.maxSets = static_cast<uint32_t>(size);
  if (vkCreateDescriptorPool(vcore_->GetDevice(), &pool_info, nullptr,
                             &pool_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool");
  }
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
  vkDestroyDescriptorPool(vcore_->GetDevice(), pool_, nullptr);
}

bool VulkanDescriptorPool::CanAllocate(
    const std::vector<VkDescriptorSetLayoutBinding> &bindings) const {
  for (auto &binding : bindings) {
    if (std::find_if(pool_sizes_.begin(), pool_sizes_.end(), [&](auto &size) {
          return size.type == binding.descriptorType &&
                 size.descriptorCount >= binding.descriptorCount;
        }) == pool_sizes_.end()) {
      return false;
    }
  }
  return true;
}

} // namespace slash