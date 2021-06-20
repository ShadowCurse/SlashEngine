#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_PIPELINE_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_PIPELINE_HPP_

#include "Core/core.hpp"
#include "vulkan_core.hpp"
#include "vulkan_descriptor_set_layout.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_shader.hpp"

namespace slash {

class VulkanPipeline {
public:
  VulkanPipeline(VulkanCore *vcore, VulkanRenderPass *render_pass,
                 VulkanSwapChain *swap_chain, VulkanShader *vertex_shader,
                 VulkanShader *fragment_shader,
                 VulkanDescriptorSetLayout *descriptor_set_layout);
  ~VulkanPipeline();

  [[nodiscard]] constexpr inline VkPipeline GetPipeline() const {
    return pipeline_;
  }
  [[nodiscard]] constexpr inline VkPipelineLayout GetPipelineLayout() const {
    return pipeline_layout_;
  }

private:
  VulkanCore *vcore_;
  VulkanRenderPass *render_pass_;
  VulkanSwapChain *swap_chain_;
  VulkanDescriptorSetLayout *descriptor_set_layout_;
  VkPipelineLayout pipeline_layout_;
  VkPipeline pipeline_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_PIPELINE_HPP_
