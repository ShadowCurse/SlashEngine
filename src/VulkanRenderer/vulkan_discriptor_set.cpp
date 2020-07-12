#include "vulkan_discriptor_set.hpp"

namespace slash {

VulkanDescriptorSet::VulkanDescriptorSet(VulkanCore *vcore,
                                         VulkanDescriptorPool *pool,
                                         VulkanDescriptorSetLayout *layout)
    : vcore_(vcore), pool_(pool), layout_(layout) {
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = pool_->GetPool();
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout_->GetLayout();
  if (vkAllocateDescriptorSets(vcore_->GetDevice(), &alloc_info,
                               &descriptor_set_) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor set");
  }
}

VulkanDescriptorSet::~VulkanDescriptorSet() {}

void VulkanDescriptorSet::Update(
    const std::vector<DescriptorInfo> &descriptors) {
  for (auto& descr: descriptors) {

  }
}

const VkDescriptorSet &VulkanDescriptorSet::GetDescriptor() const {
  return descriptor_set_;
}

} // namespace slash
