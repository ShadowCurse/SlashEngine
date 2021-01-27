#include "vulkan_swap_chain.hpp"
namespace slash {

VulkanSwapChain::VulkanSwapChain(VulkanCore *vcore,
                                 const std::vector<VulkanQueue *> &queues)
    : vcore_(vcore) {

  SwapChainSupportDetails swap_chain_support = vcore_->get_swap_chain_support();

  VkSurfaceFormatKHR surface_format =
      ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode =
      ChooseSwapPresentMode(swap_chain_support.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (image_count > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = vcore_->get_surface();
  createInfo.minImageCount = image_count;
  createInfo.imageFormat = surface_format.format;
  createInfo.imageColorSpace = surface_format.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // also can be
  // VK_IMAGE_USAGE_TRANSFER_DST_BIT

  std::vector<uint32_t> family_indices;
  bool same_family = true;
  for (uint32_t current = queues[0]->GetFamilyIndex(); auto &queue : queues) {
    if (current != queue->GetFamilyIndex())
      same_family = false;
    current = queue->GetFamilyIndex();
    family_indices.emplace_back(current);
  }

  if (!same_family) {
    createInfo.imageSharingMode =
        VK_SHARING_MODE_CONCURRENT; // Images can be used across multiple queue
    // families without explicit ownership
    // transfers.
    createInfo.queueFamilyIndexCount =
        static_cast<uint32_t>(family_indices.size());
    createInfo.pQueueFamilyIndices = family_indices.data();
  } else {
    createInfo.imageSharingMode =
        VK_SHARING_MODE_EXCLUSIVE; // An image is owned by one queue family at a
    // time and ownership must be explicitly
    // transfered before using it in another
    // queue family. This option offers the best
    // performance.
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }
  createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = present_mode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(vcore->get_device(), &createInfo, nullptr,
                           &swap_chain_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain");
  }

  format_ = surface_format.format;
  extent_2_d_ = extent;

  vkGetSwapchainImagesKHR(vcore->get_device(), swap_chain_, &image_count,
                          nullptr);
  images_.resize(image_count);
  vkGetSwapchainImagesKHR(vcore->get_device(), swap_chain_, &image_count,
                          images_.data());

  image_views_.resize(images_.size());
  for (size_t i(0); i < images_.size(); ++i) {
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = images_[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format_;
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vcore_->get_device(), &view_info, nullptr,
                          &image_views_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture views!");
    }
  }

}

VulkanSwapChain::~VulkanSwapChain() {
  for (size_t i(0); i < images_.size(); ++i) {
//    vkDestroyImage(vcore_->get_device(), images_[i], nullptr);
    vkDestroyImageView(vcore_->get_device(), image_views_[i], nullptr);
  }
  vkDestroySwapchainKHR(vcore_->get_device(), swap_chain_, nullptr);
}

VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &surface_formats) {
  for (const auto &available_format : surface_formats) {
    if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
        available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_format;
    }
  }
  return surface_formats[0];
}

VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &present_formats) {
  for (const auto &available_present_mode : present_formats) {
    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return available_present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VulkanSwapChain::ChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    auto[width, height] = vcore_->get_window_size();
    VkExtent2D actual_extent = {width, height};
    actual_extent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.width = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
  }
}

} // namespace slash
