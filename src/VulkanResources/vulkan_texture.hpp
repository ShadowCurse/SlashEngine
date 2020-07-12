#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_TEXTURE_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_TEXTURE_HPP_

#include "Core/core.hpp"
#include "VulkanRenderer/vulkan_core.hpp"

namespace slash {

class VulkanTextureSampler {
public:
  explicit VulkanTextureSampler(VulkanCore *vcore);
  ~VulkanTextureSampler();

  [[nodiscard]] constexpr inline VkSampler GetSampler() const {
    return sampler_;
  }

private:
  VulkanCore *vcore_;
  VkSampler sampler_;
};

class VulkanTexture {
public:
  VulkanTexture(VulkanCore *vcore, uint32_t width, uint32_t height,
                VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                VkMemoryPropertyFlags properties);
  ~VulkanTexture();

  [[nodiscard]] constexpr inline VkImage GetImage() const { return image_; }
  [[nodiscard]] constexpr inline VkImageView GetImageView() const {
    return image_view_;
  }
  [[nodiscard]] constexpr inline uint32_t Width() const { return width_; }
  [[nodiscard]] constexpr inline uint32_t Height() const { return height_; }

private:
  VulkanCore *vcore_;
  uint32_t width_;
  uint32_t height_;
  VkImage image_;
  VkImageView image_view_;
  VkDeviceMemory memory_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_TEXTURE_HPP_
