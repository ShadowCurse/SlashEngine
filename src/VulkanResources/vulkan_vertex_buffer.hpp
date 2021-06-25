#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_VERTEX_BUFFER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_VERTEX_BUFFER_HPP_

#include "VulkanRenderer/vulkan_core.hpp"
#include "VulkanResources/vulkan_buffer.hpp"

namespace slash {

class VulkanVertexBuffer final : public VulkanBuffer {
 public:
  VulkanVertexBuffer(VulkanCore *vcore, VkDeviceSize size,
                     uint32_t offset);

  ~VulkanVertexBuffer() final = default;

  [[nodiscard]] constexpr inline VkDeviceSize GetOffset() const {
    return offset_;
  }

 private:
  VkDeviceSize offset_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_VERTEX_BUFFER_HPP_
