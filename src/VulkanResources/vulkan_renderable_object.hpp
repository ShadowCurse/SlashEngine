#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_

#include "Core/core.hpp"
#include "VulkanRenderer/vulkan_discriptor_set.hpp"
#include "VulkanRenderer/vulkan_pipeline.hpp"
#include "vulkan_mesh.hpp"
#include "vulkan_texture.hpp"
#include "vulkan_buffer.hpp"

namespace slash {

struct VulkanRenderableObject {
  std::shared_ptr<VulkanMesh> mesh_;
  std::shared_ptr<VulkanTexture> texture_;
  std::shared_ptr<VulkanBuffer> transform_;
  VulkanDescriptorSet *descriptor_set_;
  VulkanPipeline* pipeline_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_
