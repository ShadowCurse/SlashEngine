#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_HPP_

#include "VulkanResources/vulkan_renderable_object.hpp"
#include "vulkan_command_pool.hpp"
#include "vulkan_core.hpp"

namespace slash {

class VulkanCommandBuffer {
 public:
  VulkanCommandBuffer(VulkanCore *vcore, VulkanCommandPool *command_pool,
                      VkCommandBufferLevel level);
  ~VulkanCommandBuffer();

  void Begin();
  void End();
  void BeginRenderPass();
  void EndRenderPass();
  void BindPipeline();

  void AddRenderableObject(VulkanRenderableObject &object);

  [[nodiscard]] constexpr VkCommandBuffer GetBuffer() const { return buffer_; };

  enum class State { NotStarted, Started, InRenderPass, Ended };

 private:
  VulkanCore *vcore_;
  VulkanCommandPool *command_pool_;
  VkCommandBufferLevel level_;
  State state_;
  VkCommandBuffer buffer_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_BUFFER_HPP_
