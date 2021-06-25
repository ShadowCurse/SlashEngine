#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SHADER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SHADER_HPP_

#include "vulkan_core.hpp"

namespace slash {
class VulkanShader {
 public:
  VulkanShader(VulkanCore *vcore, const std::string &path, VkShaderStageFlagBits stage);
  VulkanShader(VulkanCore *vcore, const std::vector<char> &code);

  ~VulkanShader();

  [[nodiscard]] auto pipeline_shader_stage_create_info() const -> VkPipelineShaderStageCreateInfo;
 private:
  VulkanCore *vcore_;
  VkShaderStageFlagBits stage_;
  VkShaderModule shader_;
};
}

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SHADER_HPP_
