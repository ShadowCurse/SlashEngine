#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DISCRIPTOR_SET_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DISCRIPTOR_SET_HPP_

#include "Core/core.hpp"
#include "VulkanResources/vulkan_buffer.hpp"
#include "vulkan_core.hpp"
#include "vulkan_descriptor_pool.hpp"
#include "vulkan_descriptor_set_layout.hpp"
#include "VulkanResources/vulkan_texture.hpp"

namespace slash {

enum class DescriptorType { Buffer, Image };

struct DescriptorInfo {
  DescriptorType type;
  VulkanBuffer *buffer;
  VulkanTexture *texture;
};

class VulkanDescriptorSet {
public:
  VulkanDescriptorSet(VulkanCore *vcore, VulkanDescriptorPool *pooli,
                      VulkanDescriptorSetLayout *layout);
  ~VulkanDescriptorSet();

  void Update(const std::vector<DescriptorInfo>& descriptors);
  void AddBuffer();
  void AddImage();
  [[nodiscard]] const VkDescriptorSet &GetDescriptor() const;

private:
  VulkanCore *vcore_;
  VulkanDescriptorPool *pool_;
  VulkanDescriptorSetLayout *layout_;
  VkDescriptorSet descriptor_set_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_DISCRIPTOR_SET_HPP_
