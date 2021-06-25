#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_POOL_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_POOL_HPP_

#include "GLFW/glfw3.h"
#include "vulkan_core.hpp"

namespace slash {

class VulkanCommandPool {
 public:
  VulkanCommandPool(VulkanCore *vcore, uint32_t queue_family_index,
                    VkCommandPoolCreateFlags flags);
  ~VulkanCommandPool();
  [[nodiscard]] const VkCommandPool &GetPool() const;
  void reset();

 private:
  VulkanCore *vcore_;
  VkCommandPool pool_;
  uint32_t queue_family_index_;
  VkCommandPoolCreateFlags flags_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_COMMAND_POOL_HPP_
