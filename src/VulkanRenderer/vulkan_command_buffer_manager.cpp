#include "vulkan_command_buffer_manager.hpp"

namespace slash {

VulkanCommandBufferManager::VulkanCommandBufferManager(
    VulkanCore *vcore, uint32_t queue_family_index)
    : vcore_(vcore), queue_family_index_(queue_family_index),
      pool_(vcore_, queue_family_index,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT), page_(false) {}

std::shared_ptr<VulkanCommandBuffer>
VulkanCommandBufferManager::CreateBuffer() {
  auto buffer = std::make_shared<VulkanCommandBuffer>(
      vcore_, &pool_, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  command_buffers_[page_].push_back(buffer);
  return buffer;
}

void VulkanCommandBufferManager::FreeBuffer(
    std::shared_ptr<VulkanCommandBuffer> buffer) {
  if (auto result =
          std::find(command_buffers_[page_].begin(), command_buffers_[page_].end(), buffer);
      result != command_buffers_[page_].end()) {
    command_buffers_[page_].erase(result);
  }
}

std::vector<VkCommandBuffer>
VulkanCommandBufferManager::GetBuffers() {
  std::vector<VkCommandBuffer> ret(command_buffers_[page_].size());
  for (size_t i(0); i < command_buffers_[page_].size(); ++i) {
    ret[i] = command_buffers_[page_][i]->GetBuffer();
  }
  page_ = !page_;
  command_buffers_[page_].clear();
  return ret;
}

} // namespace slash