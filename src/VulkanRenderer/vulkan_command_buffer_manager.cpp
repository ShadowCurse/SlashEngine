#include "vulkan_command_buffer_manager.hpp"

namespace slash {

VulkanCommandBufferManager::VulkanCommandBufferManager(
    VulkanCore *vcore, uint32_t queue_family_index)
    : vcore_(vcore), queue_family_index_(queue_family_index),
      pool_(vcore_, queue_family_index,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
      pools_{{{vcore_, queue_family_index,
               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT},
              {vcore_, queue_family_index,
               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT},
              {vcore_, queue_family_index,
               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT}}} {}

VulkanCommandBufferManager::~VulkanCommandBufferManager() {
  vkResetCommandPool(vcore_->get_device(), pool_.GetPool(), 0);
  for (auto &pool : pools_)
    vkResetCommandPool(vcore_->get_device(), pool.GetPool(), 0);
}

std::shared_ptr<VulkanCommandBuffer>
VulkanCommandBufferManager::CreateBuffer() {
  auto buffer = std::make_shared<VulkanCommandBuffer>(
      vcore_, &pool_, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  command_buffers_.push_back(buffer);
  return buffer;
}

void VulkanCommandBufferManager::FreeBuffer(
    std::shared_ptr<VulkanCommandBuffer> buffer) {
  if (auto result =
        std::find(command_buffers_.begin(), command_buffers_.end(), buffer);
      result != command_buffers_.end()) {
    command_buffers_.erase(result);
  }
}

std::vector<VkCommandBuffer> VulkanCommandBufferManager::GetBuffers() {
  std::vector<VkCommandBuffer> ret;
  for (const auto &buffer : frame_command_buffers_[current_pool]) {
    ret.emplace_back(buffer->GetBuffer());
  }
  return ret;
}

void VulkanCommandBufferManager::SwitchFrame() {
  current_pool = (current_pool + 1) % pools_.size();
  frame_command_buffers_[current_pool].clear();
  pools_[current_pool].reset();
}

std::shared_ptr<VulkanCommandBuffer>
VulkanCommandBufferManager::CreateFrameCommandBuffer() {
  auto buffer = std::make_shared<VulkanCommandBuffer>(
      vcore_, &pools_[current_pool], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  frame_command_buffers_[current_pool].push_back(buffer);
  return buffer;
}
void VulkanCommandBufferManager::FreeFrameCommandBuffer(
    std::shared_ptr<VulkanCommandBuffer> buffer) {}

} // namespace slash