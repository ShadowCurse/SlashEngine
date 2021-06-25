#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_IMGUI_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_IMGUI_HPP_

#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_vulkan.h"

namespace slash {

using ImGuiDrawFn = std::function<void()>;

class VulkanImGui {
 public:
  VulkanImGui() = default;
  ~VulkanImGui() = default;

  void Init(GLFWwindow *window, VkInstance instance,
            VkPhysicalDevice physical_device, VkDevice device,
            uint32_t queue_family, VkQueue graphics_queue,
            VkPipelineCache pipeline_cache, VkFormat render_pass_color_format,
            VkFormat render_pass_depth_format, uint32_t image_count);

  void Destroy();

  void SetDrawFn(ImGuiDrawFn &fn);

  void UpdateCommandBuffers(VkFramebuffer framebuffer, VkExtent2D extend);
  VkCommandBuffer CommandBuffer();
  void SetMinimalImageCount(uint32_t image_count);

 private:
  void CreateDescriptorPool();
  void CreateCommandPool();
  void CreateCommandBuffers();
  void CreateRenderPass();

  ImGuiDrawFn draw_fn_;

  GLFWwindow *window_;
  VkInstance instance_;
  VkPhysicalDevice physical_device_;
  VkDevice device_;
  uint32_t queue_family_;
  VkQueue graphics_queue_;
  VkPipelineCache pipeline_cache_;
  VkFormat render_pass_color_format_;
  VkFormat render_pass_depth_format_;
  uint32_t image_count_;

  VkDescriptorPool descriptor_pool_;
  VkCommandPool command_pool_;
  VkCommandBuffer command_buffer_;
  VkRenderPass render_pass_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_IMGUI_HPP_
