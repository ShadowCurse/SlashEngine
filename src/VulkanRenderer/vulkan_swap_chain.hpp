#ifndef SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SWAP_CHAIN_HPP_
#define SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SWAP_CHAIN_HPP_

#include "GLFW/glfw3.h"
#include "vulkan_core.hpp"
#include "vulkan_fence.hpp"
#include "vulkan_queue.hpp"
#include "vulkan_semaphore.hpp"

namespace slash {

class VulkanSwapChain {
public:
  VulkanSwapChain(VulkanCore *vcore, const std::vector<VulkanQueue *> &queues);
  ~VulkanSwapChain();
  VkResult AquireImage(uint32_t timeout, const VulkanSemaphore &semaphore,
                       const VulkanFence &fence, uint32_t *p_image_index);

  [[nodiscard]] constexpr VkSwapchainKHR GetSwapChain() const { return swap_chain_; }
  [[nodiscard]] constexpr VkFormat GetFormat() const { return format_; }
  [[nodiscard]] constexpr const VkExtent2D &GetExtent() const { return extent_2_d_; }
  [[nodiscard]] constexpr const std::vector<VkImageView>& GetImageViews() const { return image_views_; }

private:
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &surface_formats);
  VkPresentModeKHR
  ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &present_formats);
  VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);

  VulkanCore *vcore_;
  VkSwapchainKHR swap_chain_;
  VkFormat format_;
  VkExtent2D extent_2_d_;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRENDERER_VULKAN_SWAP_CHAIN_HPP_
