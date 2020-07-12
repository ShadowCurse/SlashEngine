#include "vulkan_descriptor_manager.hpp"

namespace slash {

VulkanDescriptorManager::VulkanDescriptorManager(VulkanCore *vcore,
                                                 uint32_t default_pool_size)
    : vcore_(vcore), default_pool_size_(default_pool_size) {}

VulkanDescriptorManager::~VulkanDescriptorManager() {}

VulkanDescriptorSetLayout *
VulkanDescriptorManager::CreateDescriptorSetLayout() {
  layouts_.emplace_back(VulkanDescriptorSetLayout(vcore_));
  return &layouts_.back();
}

VulkanDescriptorSet *VulkanDescriptorManager::CreateDescriptorSet(
    VulkanDescriptorSetLayout *layout) {

  const auto &bindings = layout->GetBindings();
  for (auto &pool : pools_) {
    if (pool->CanAllocate(bindings)) {
      sets_.emplace_back(VulkanDescriptorSet(vcore_, pool.get(), layout));
      return &sets_.back();
    }
  }
  pools_.emplace_back(std::make_shared<VulkanDescriptorPool>(
      vcore_, bindings, default_pool_size_));
  sets_.emplace_back(VulkanDescriptorSet(vcore_, pools_.back().get(), layout));
  return &sets_.back();
}

} // namespace slash