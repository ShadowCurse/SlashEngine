#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_

#include "Core/core.hpp"
#include "Core/log.hpp"
#include "VulkanRenderer/vulkan_discriptor_set.hpp"
#include "VulkanRenderer/vulkan_pipeline.hpp"
#include "vulkan_mesh.hpp"
#include "vulkan_texture.hpp"
#include "vulkan_buffer.hpp"

namespace slash {

struct VulkanRenderableObject {
  explicit VulkanRenderableObject() = default;

  explicit VulkanRenderableObject(
      std::unique_ptr<VulkanMesh> mesh,
      std::unique_ptr<VulkanTexture> texture,
      std::unique_ptr<VulkanBuffer> transform,
      VulkanDescriptorSet *descriptor_set,
      VulkanPipeline *pipeline
  )
      : mesh_{std::move(mesh)},
        texture_{std::move(texture)},
        transform_{std::move(transform)},
        descriptor_set_{descriptor_set},
        pipeline_{pipeline} {}

  VulkanRenderableObject(VulkanRenderableObject &&other)
  noexcept: mesh_{std::move(other.mesh_)},
            texture_{std::move(other.texture_)},
            transform_{std::move(other.transform_)},
            descriptor_set_{other.descriptor_set_},
            pipeline_{other.pipeline_} {}

  auto operator=(VulkanRenderableObject &&other) noexcept -> VulkanRenderableObject & {
    mesh_ = std::move(other.mesh_);
    texture_ = std::move(other.texture_);
    transform_ = std::move(other.transform_);
    descriptor_set_ = other.descriptor_set_;
    pipeline_ = other.pipeline_;
    return *this;
  }

  std::unique_ptr<VulkanMesh> mesh_;
  std::unique_ptr<VulkanTexture> texture_;
  std::unique_ptr<VulkanBuffer> transform_;
  VulkanDescriptorSet *descriptor_set_;
  VulkanPipeline *pipeline_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_
