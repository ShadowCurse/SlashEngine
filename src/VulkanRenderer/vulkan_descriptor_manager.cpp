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
      auto set =
          std::make_shared<VulkanDescriptorSet>(vcore_, pool.get(), layout);
      sets_.emplace_back(set);
      return set.get();
    }
  }
  auto pool = std::make_shared<VulkanDescriptorPool>(vcore_, bindings,
                                                     default_pool_size_);
  auto set = std::make_shared<VulkanDescriptorSet>(vcore_, pool.get(), layout);
  pools_.emplace_back(pool);
  sets_.emplace_back(set);
  return set.get();
}

} // namespace slash