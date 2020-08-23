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
  void SwitchFrame();
  [[nodiscard]] std::shared_ptr<VulkanCommandBuffer> CreateFrameCommandBuffer();
  void FreeFrameCommandBuffer(std::shared_ptr<VulkanCommandBuffer> buffer);

private:
  VulkanCore *vcore_;
  uint32_t queue_family_index_;
  std::vector<std::shared_ptr<VulkanCommandBuffer>> command_buffers_;
  VulkanCommandPool pool_; // should be 3 pools

  std::array<std::vector<std::shared_ptr<VulkanCommandBuffer>>, 3>
      frame_command_buffers_;
  std::array<VulkanCommandPool, 3> pools_;
  uint8_t current_pool = 0;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_MANAGER_HPP_
