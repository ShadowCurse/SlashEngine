#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_FRAME_BUFFER_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_FRAME_BUFFER_HPP_

#include "Core/core.hpp"
#include "VulkanRenderer/vulkan_renderpass.hpp"
#include "VulkanRenderer/vulkan_swap_chain.hpp"
#include "vulkan_texture.hpp"

namespace slash {

class VulkanFrameBuffer {
public:
  VulkanFrameBuffer(VulkanCore *vcore, VulkanRenderPass *render_pass,
                   uint32_t width, uint32_t height,
                   const std::vector<VkImageView> &attachments);
  ~VulkanFrameBuffer();

  [[nodiscard]] constexpr inline VkFramebuffer GetFrameBuffer() const {
    return frame_buffer_;
  }

private:
  VulkanCore *vcore_;
  VulkanRenderPass *render_pass_;
  uint32_t width_;
  uint32_t height_;
  VkFramebuffer frame_buffer_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_FRAME_BUFFER_HPP_
