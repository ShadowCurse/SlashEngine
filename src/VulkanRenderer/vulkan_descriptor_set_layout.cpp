#include "vulkan_descriptor_set_layout.hpp"

namespace slash {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanCore *vcore)
    : vcore_(vcore) {}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(vcore_->get_device(), layout_, nullptr);
}

void VulkanDescriptorSetLayout::AddBinding(VkDescriptorType type,
                                           VkShaderStageFlagBits stage) {
  VkDescriptorSetLayoutBinding binding = {};
  binding.binding = static_cast<uint32_t>(bindings_.size());
  binding.descriptorType = type;
  binding.descriptorCount = 1;
  binding.stageFlags = stage;
  binding.pImmutableSamplers = nullptr;
  bindings_.emplace_back(binding);
}

void VulkanDescriptorSetLayout::CreateLayout() {
  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(bindings_.size());
  layout_info.pBindings = bindings_.data();
  if (vkCreateDescriptorSetLayout(vcore_->get_device(), &layout_info, nullptr,
                                  &layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout");
  }
}
const VkDescriptorSetLayout &VulkanDescriptorSetLayout::GetLayout() const {
  return layout_;
}

const std::vector<VkDescriptorSetLayoutBinding> &
VulkanDescriptorSetLayout::GetBindings() const {
  return bindings_;
}

} // namespace slash