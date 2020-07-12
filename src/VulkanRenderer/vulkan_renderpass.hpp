#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_RENDERPASS_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_RENDERPASS_HPP_

#include "Core/core.hpp"
#include "vulkan_core.hpp"
#include "vulkan_swap_chain.hpp"

namespace slash {

class VulkanRenderPassDescription {

  VkAttachmentDescription CreateAttachemt(VkFormat format, VkSampleCountFlagBits samples,
                     VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op,
                     VkAttachmentLoadOp stencil_load_op,
                     VkAttachmentStoreOp stencil_store_op,
                     VkImageLayout initial_layout, VkImageLayout final_layout);

  VkSubpassDescription CreateSubpass(VkSubpassDescriptionFlags flags,
                  VkPipelineBindPoint pipelineBindPoint,
                  uint32_t inputAttachmentCount,
                  const VkAttachmentReference *pInputAttachments,
                  uint32_t colorAttachmentCount,
                  const VkAttachmentReference *pColorAttachments,
                  const VkAttachmentReference *pResolveAttachments,
                  const VkAttachmentReference *pDepthStencilAttachment,
                  uint32_t preserveAttachmentCount,
                  const uint32_t *pPreserveAttachments);

  std::vector<VkAttachmentDescription> attachments_;
  std::vector<VkAttachmentReference> references_;
  VkSubpassDescription subpass_;
  VkSubpassDependency dependency_;
};

class VulkanRenderPass {
public:
  VulkanRenderPass(VulkanCore *vcore, VulkanSwapChain* swap_chain);
  ~VulkanRenderPass();

  const VkRenderPass GetRenderPass() const;

private:
  VulkanCore* vcore_;
  VulkanSwapChain* swap_chain_;
  VkRenderPass render_pass_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_RENDERPASS_HPP_
