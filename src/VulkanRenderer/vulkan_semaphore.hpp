#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SEMAPHORE_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SEMAPHORE_HPP_

#include "vulkan_core.hpp"

namespace slash {

class VulkanSemaphore {
 public:
  explicit VulkanSemaphore(VulkanCore *vcore);
  ~VulkanSemaphore();

  [[nodiscard]] VkSemaphore GetSemaphore() const;

 private:
  VulkanCore *vcore_;
  VkSemaphore semaphore_;
};

}

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SEMAPHORE_HPP_
