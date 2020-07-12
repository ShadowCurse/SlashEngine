#include "vulkan_frame_buffer.hpp"

namespace slash {

VulkanFrameBuffer::VulkanFrameBuffer(
    VulkanCore *vcore, VulkanRenderPass *render_pass,
    uint32_t width, uint32_t height,
    const std::vector<VkImageView> &attachments)
    : vcore_(vcore), render_pass_(render_pass), width_(width), height_(height) {
//  std::vector<VkImageView> attachments_view;
//  std::for_each(attachments.begin(), attachments.end(), [&](auto &image) {
//    attachments_view.emplace_back(image->GetImageView());
//  });
  VkFramebufferCreateInfo framebuffer_info = {};
  framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebuffer_info.renderPass = render_pass_->GetRenderPass();
  framebuffer_info.attachmentCount =
      static_cast<uint32_t>(attachments.size());
  framebuffer_info.pAttachments = attachments.data();
  framebuffer_info.width = width_;
  framebuffer_info.height = height_;
  framebuffer_info.layers = 1;
  if (vkCreateFramebuffer(vcore_->GetDevice(), &framebuffer_info, nullptr,
                          &frame_buffer_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer");
  }
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  vkDestroyFramebuffer(vcore_->GetDevice(), frame_buffer_, nullptr);
}

} // namespace slash