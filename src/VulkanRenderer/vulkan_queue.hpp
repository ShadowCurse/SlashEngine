#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_QUEUE_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_QUEUE_HPP_

#include "Core/core.hpp"
#include "GLFW/glfw3.h"
#include "vulkan_core.hpp"

namespace slash {

class VulkanQueue {
 public:
  VulkanQueue(VulkanCore *vcore, uint32_t family_index);

  [[nodiscard]] uint32_t GetFamilyIndex() const;
  [[nodiscard]] VkQueue GetQueue() const;
//  void Submit(VulkanCommandBuffers* command_buffers, uint32_t command_buffer_num, VulkanSemaphores* wait_semaphores, uint32_t wait_semaphores_num);

 private:
  VulkanCore *vcore_;
  uint32_t family_index_;
  VkQueue queue_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_QUEUE_HPP_
