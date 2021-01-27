#include "vulkan_texture.hpp"

namespace slash {

VulkanTextureSampler::VulkanTextureSampler(VulkanCore *vcore) : vcore_(vcore) {
  VkSamplerCreateInfo sampler_info = {};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_TRUE;
  sampler_info.maxAnisotropy = 16;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  if (vkCreateSampler(vcore->get_device(), &sampler_info, nullptr, &sampler_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler");
  }
}

VulkanTextureSampler::~VulkanTextureSampler() {
  vkDestroySampler(vcore_->get_device(), sampler_, nullptr);
}

VulkanTexture::VulkanTexture(VulkanCore *vcore, uint32_t width, uint32_t height,
                             VkFormat format, VkImageTiling tiling,
                             VkImageUsageFlags usage,
                             VkMemoryPropertyFlags properties)
    : vcore_(vcore), width_(width), height_(height) {
  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(vcore_->get_device(), &image_info, nullptr, &image_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create image");
  }

  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(vcore_->get_device(), image_,
                               &memory_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = memory_requirements.size;
  alloc_info.memoryTypeIndex = vcore_->find_memory_type_index(
      memory_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(vcore_->get_device(), &alloc_info, nullptr, &memory_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate memory");
  }
  vkBindImageMemory(vcore_->get_device(), image_, memory_, 0);

  VkImageViewCreateInfo view_info = {};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image_;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  if (vkCreateImageView(vcore_->get_device(), &view_info, nullptr,
                        &image_view_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture views!");
  }
}

VulkanTexture::~VulkanTexture() {
  vkDestroyImageView(vcore_->get_device(), image_view_, nullptr);
  vkDestroyImage(vcore_->get_device(), image_, nullptr);
  vkFreeMemory(vcore_->get_device(), memory_, nullptr);
}

} // namespace slash
