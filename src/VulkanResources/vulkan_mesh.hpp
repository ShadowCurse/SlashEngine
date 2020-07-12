#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_MESH_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_MESH_HPP_

#include "vulkan_index_buffer.hpp"
#include "vulkan_vertex_buffer.hpp"

namespace slash {

struct VulkanMesh {
  VulkanVertexBuffer *vertex_buffer;
  VulkanIndexBuffer *index_buffer;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_MESH_HPP_
