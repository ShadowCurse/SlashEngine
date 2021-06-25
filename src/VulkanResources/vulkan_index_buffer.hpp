#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_INDEX_BUFFER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_INDEX_BUFFER_HPP_

#include "vulkan_buffer.hpp"

namespace slash {

class VulkanIndexBuffer final : public VulkanBuffer {
 public:
  VulkanIndexBuffer(VulkanCore *vcore, VkDeviceSize buffer_size,
                    uint32_t elements_num);

  ~VulkanIndexBuffer() final = default;

  [[nodiscard]] uint32_t GetElementsNum() const;

 private:
  uint32_t elements_num_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_INDEX_BUFFER_HPP_
