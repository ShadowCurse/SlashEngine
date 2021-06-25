#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_FENCE_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_FENCE_HPP_

#include "vulkan_core.hpp"

namespace slash {

class VulkanFence {
 public:
  explicit VulkanFence(VulkanCore *vcore);
  ~VulkanFence();

  [[nodiscard]] const VkFence &GetFence() const;

 private:
  VulkanCore *vcore_;
  VkFence fence_;
};

}
#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_FENCE_HPP_
