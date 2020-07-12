#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP_

#include "Core/core.hpp"
#include "vulkan_core.hpp"

namespace slash {

class VulkanDescriptorSetLayout {
public:
  VulkanDescriptorSetLayout(VulkanCore* vcore);
  ~VulkanDescriptorSetLayout();

  void AddBinding(VkDescriptorType type, VkShaderStageFlagBits stage);
  void CreateLayout();
  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;

  const VkDescriptorSetLayout& GetLayout() const;

private:
  VulkanCore* vcore_;
  std::vector<VkDescriptorSetLayoutBinding> bindings_;
  VkDescriptorSetLayout layout_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP_
