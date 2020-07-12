#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_MANAGER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_MANAGER_HPP_

#include "Core/core.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_command_pool.hpp"

namespace slash {

class VulkanCommandBufferManager {
public:
  VulkanCommandBufferManager(VulkanCore *vcore, uint32_t queue_family_index);
  ~VulkanCommandBufferManager() = default;

  [[nodiscard]] std::shared_ptr<VulkanCommandBuffer> CreateBuffer();
  void FreeBuffer(std::shared_ptr<VulkanCommandBuffer> buffer);
  [[nodiscard]] std::vector<VkCommandBuffer> GetBuffers();

private:
  VulkanCore *vcore_;
  uint32_t queue_family_index_;
  std::array<std::vector<std::shared_ptr<VulkanCommandBuffer>>, 2>
      command_buffers_;
  bool page_;
  VulkanCommandPool pool_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_MANAGER_HPP_
