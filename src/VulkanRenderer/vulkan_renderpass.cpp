#include "vulkan_renderpass.hpp"

namespace slash {

VulkanRenderPass::VulkanRenderPass(VulkanCore *vcore,
                                   VulkanSwapChain *swap_chain)
    : vcore_(vcore), swap_chain_(swap_chain) {

  VkAttachmentDescription color_attachment = {};
  color_attachment.format = swap_chain->GetFormat();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  //      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //
  //      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  // //
  // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth_attachment = {};
  depth_attachment.format = vcore->FindSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D32_SFLOAT_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  std::array<VkSubpassDescription, 1> subpasses = {};
  subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpasses[0].colorAttachmentCount = 1;
  subpasses[0].pColorAttachments = &color_attachment_ref;
  subpasses[0].pDepthStencilAttachment = &depth_attachment_ref;

  std::array<VkSubpassDependency, 1> dependency = {};
  dependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency[0].dstSubpass = 0;
  dependency[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency[0].srcAccessMask = 0;
  dependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachemnts = {color_attachment,
                                                        depth_attachment};

  VkRenderPassCreateInfo create_info;
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = static_cast<uint32_t>(attachemnts.size());
  create_info.pAttachments = attachemnts.data();
  create_info.subpassCount = static_cast<uint32_t>(subpasses.size());
  create_info.pSubpasses = subpasses.data();
  create_info.dependencyCount = static_cast<uint32_t>(dependency.size());
  create_info.pDependencies = dependency.data();
  create_info.flags = 0;

  if (vkCreateRenderPass(vcore->GetDevice(), &create_info, nullptr,
                         &render_pass_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass");
  }
}

VulkanRenderPass::~VulkanRenderPass() {
  vkDestroyRenderPass(vcore_->GetDevice(), render_pass_, nullptr);
}

VkRenderPass VulkanRenderPass::GetRenderPass() const {
  return render_pass_;
}

} // namespace slash