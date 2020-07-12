#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_POOL_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_POOL_HPP_

#include "Core/core.hpp"
#include "vulkan_core.hpp"

namespace slash {

class VulkanDescriptorPool {
public:
  VulkanDescriptorPool(VulkanCore* vcore, const std::vector<VkDescriptorSetLayoutBinding> bindings, uint32_t size);
  ~VulkanDescriptorPool();

  [[nodiscard]] bool CanAllocate(const std::vector<VkDescriptorSetLayoutBinding>& bindings) const;
  [[nodiscard]] constexpr VkDescriptorPool GetPool() const { return pool_; }

private:
  VulkanCore* vcore_;
  VkDescriptorPool pool_;
  std::vector<VkDescriptorPoolSize> pool_sizes_;
  const uint32_t max_sets_;
};

}

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_POOL_HPP_
