#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_MESH_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_MESH_HPP_

#include "Core/core.hpp"
#include "vulkan_index_buffer.hpp"
#include "vulkan_vertex_buffer.hpp"

namespace slash {

class VulkanMesh {
 public:
  VulkanMesh() = default;
  VulkanMesh(std::shared_ptr<VulkanVertexBuffer> vertex_buffer, std::shared_ptr<VulkanIndexBuffer> index_buffer)
      : vertex_buffer_(std::move(vertex_buffer)), index_buffer_(std::move(index_buffer)) {}

  VulkanMesh(VulkanMesh &other) {
    vertex_buffer_ = std::move(other.vertex_buffer_);
    index_buffer_ = std::move(other.index_buffer_);
  }
  VulkanMesh& operator=(const VulkanMesh &other) = default;

  VulkanVertexBuffer *VertexBuffer() { return vertex_buffer_.get(); }
  VulkanIndexBuffer *IndexBuffer() { return index_buffer_.get(); }

 private:
  std::shared_ptr<VulkanVertexBuffer> vertex_buffer_;
  std::shared_ptr<VulkanIndexBuffer> index_buffer_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_MESH_HPP_
