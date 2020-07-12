#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_

#include "Core/core.hpp"
#include "GameResources/model.hpp"
#include "VulkanRenderer/vulkan_discriptor_set.hpp"
#include "VulkanRenderer/vulkan_pipeline.hpp"
#include "vulkan_mesh.hpp"

namespace slash {

struct VulkanRenderableObject {
  VulkanMesh *mesh_;
  VulkanDescriptorSet *descriptor_set_;
  VulkanPipeline* pipeline_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RENDERABLE_OBJECT_HPP_
