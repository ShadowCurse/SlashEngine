#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_MANAGER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_MANAGER_HPP_

#include "Core/core.hpp"
#include "vulkan_core.hpp"
#include "vulkan_descriptor_pool.hpp"
#include "vulkan_descriptor_set_layout.hpp"
#include "vulkan_discriptor_set.hpp"

namespace slash {

class VulkanDescriptorManager {
public:
  VulkanDescriptorManager(VulkanCore *vcore, uint32_t default_pool_size = 64);
  ~VulkanDescriptorManager();

  [[nodiscard]] VulkanDescriptorSetLayout *CreateDescriptorSetLayout();
  [[nodiscard]] VulkanDescriptorSet *
  CreateDescriptorSet(VulkanDescriptorSetLayout *layout);

private:
  VulkanCore *vcore_;
  std::vector<VulkanDescriptorSetLayout> layouts_;
  std::vector<std::shared_ptr<VulkanDescriptorSet>> sets_;
  std::vector<std::shared_ptr<VulkanDescriptorPool>> pools_;
  uint32_t default_pool_size_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_MANAGER_HPP_
