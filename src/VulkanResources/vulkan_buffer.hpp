#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_BUFFER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_BUFFER_HPP_

#include "Core/core.hpp"
#include "VulkanRenderer/vulkan_core.hpp"

namespace slash {

class VulkanBuffer {
public:
  VulkanBuffer() = default;
  VulkanBuffer(VulkanCore *vcore, VkDeviceSize buffer_size,
               VkBufferUsageFlags usage_flags,
               VkMemoryPropertyFlags memory_property_flags);
  virtual ~VulkanBuffer();

  [[nodiscard]] VkBuffer GetBuffer() const;
  [[nodiscard]] VkDeviceMemory GetMemory() const;
  [[nodiscard]] VkDeviceSize GetBufferSize() const;

protected:
  VulkanCore *vcore_;
  VkBuffer buffer_;
  VkDeviceMemory memory_;
  VkDeviceSize buffer_size_;
};

class VulkanStagingBuffer final : public VulkanBuffer {
public:
  VulkanStagingBuffer(VulkanCore* vcore, VkDeviceSize buffer_size);
  ~VulkanStagingBuffer() final = default;
};

} // namespace slash
#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_BUFFER_HPP_
