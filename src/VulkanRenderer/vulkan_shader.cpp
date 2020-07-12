#include "vulkan_shader.hpp"

namespace slash {

VulkanShader::VulkanShader(VulkanCore *vcore, const std::string &path)
    : vcore_(vcore) {
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
  if (vkCreateShaderModule(vcore_->GetDevice(), &create_info, nullptr,
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
  if (vkCreateShaderModule(vcore_->GetDevice(), &create_info, nullptr,
                           &shader_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
}

VulkanShader::~VulkanShader() {
  vkDestroyShaderModule(vcore_->GetDevice(), shader_, nullptr);
}

VkShaderModule VulkanShader::GetShader() const { return shader_; }

} // namespace slash