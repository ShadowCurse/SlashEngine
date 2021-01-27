#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_RENDERER_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_RENDERER_HPP_

#include "Core/core.hpp"
#include "WindowModule/window.hpp"
#include "VulkanResources/vulkan_frame_buffer.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_command_buffer_manager.hpp"
#include "vulkan_command_pool.hpp"
#include "vulkan_core.hpp"
#include "vulkan_descriptor_manager.hpp"
#include "vulkan_fence.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_queue.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_semaphore.hpp"
#include "vulkan_swap_chain.hpp"

namespace slash {

class VulkanRenderer {
 public:
  explicit VulkanRenderer(Window *window);
  ~VulkanRenderer();

  void new_frame();
  void draw_frame(double time);
  void wait_idle();

  [[nodiscard]] constexpr inline auto get_core() const -> VulkanCore * { return vcore_; }
  [[nodiscard]] constexpr inline auto get_pipeline() const -> VulkanPipeline * {
    return pipeline_;
  }
  [[nodiscard]] auto begin_one_time_command() const -> std::shared_ptr<VulkanCommandBuffer>;
  void end_one_time_command(std::shared_ptr<VulkanCommandBuffer> command_buffer);

  [[nodiscard]] auto create_descriptor_set() const -> VulkanDescriptorSet *;

  [[nodiscard]] auto start_render_command() const -> std::shared_ptr<VulkanCommandBuffer> ;
  void end_render_command(std::shared_ptr<VulkanCommandBuffer> buffer);

 private:
  void recreate_swap_chain();

 private:
  VulkanCore* vcore_;
  VulkanQueue* graphics_queue_;
  VulkanQueue* present_queue_;
  VulkanSwapChain* swap_chain_;
  VulkanRenderPass* render_pass_;
  VulkanDescriptorSetLayout* descriptor_set_layout_;
  VulkanPipeline* pipeline_;

  std::shared_ptr<VulkanDescriptorManager> descriptor_manager_;
  std::shared_ptr<VulkanCommandBufferManager>command_buffer_manager_;

  VulkanTexture *depth_;
  std::vector<VulkanFrameBuffer *> frame_buffers_;
  std::vector<VulkanFence *> inflight_fences_;
  std::vector<VulkanSemaphore *> image_available_semaphores_;
  std::vector<VulkanSemaphore *> render_finished_semaphores_;

  uint32_t current_frame_ = 0;
  uint32_t current_index_ = -1;
  const uint32_t max_frames_ = 2;
};

} // namespace slash
#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_RENDERER_HPP_
