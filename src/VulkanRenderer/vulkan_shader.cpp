#include "vulkan_shader.hpp"

namespace slash {

VulkanShader::VulkanShader(VulkanCore *vcore, const std::string &path, VkShaderStageFlagBits stage)
    : vcore_{vcore},
      stage_{stage} {
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open file");
  }
  size_t size = static_cast<size_t>(file.tellg());
  std::vector<char> code(size);
  file.seekg(0);
  file.read(code.data(), static_cast<std::streamsize>(size));
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());
  if (vkCreateShaderModule(vcore_->get_device(), &create_info, nullptr,
                           &shader_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
}

VulkanShader::VulkanShader(VulkanCore *vcore, const std::vector<char> &code)
    : vcore_(vcore) {
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());
  if (vkCreateShaderModule(vcore_->get_device(), &create_info, nullptr,
                           &shader_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
}

VulkanShader::~VulkanShader() {
  vkDestroyShaderModule(vcore_->get_device(), shader_, nullptr);
}

auto VulkanShader::pipeline_shader_stage_create_info() const -> VkPipelineShaderStageCreateInfo {
  VkPipelineShaderStageCreateInfo shader_stage_info = {};
  shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_info.stage = stage_;
  shader_stage_info.module = shader_;
  shader_stage_info.pName = "main";
  shader_stage_info.pSpecializationInfo = nullptr;
  return shader_stage_info;
}

} // namespace slash