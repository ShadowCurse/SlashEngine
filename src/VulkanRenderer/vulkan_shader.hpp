#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SHADER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SHADER_HPP_

#include "Core/core.hpp"
#include "vulkan_core.hpp"

namespace slash {
class VulkanShader {
public:
  VulkanShader(VulkanCore* vcore, const std::string& path);
  VulkanShader(VulkanCore* vcore, const std::vector<char>& code);

  VkShaderModule GetShader() const;

  ~VulkanShader();
private:
  VulkanCore* vcore_;
  VkShaderModule shader_;
};
}

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SHADER_HPP_
