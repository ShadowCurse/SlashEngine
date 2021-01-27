#include "vulkan_command_buffer.hpp"

namespace slash {

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCore *vcore,
                                         VulkanCommandPool *command_pool,
                                         VkCommandBufferLevel level)
    : vcore_(vcore), command_pool_(command_pool), level_(level) {
  VkCommandBufferAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandBufferCount = 1;
  allocate_info.commandPool = command_pool_->GetPool();
  allocate_info.level = level_;
  allocate_info.pNext = nullptr;
  if (vkAllocateCommandBuffers(vcore->get_device(), &allocate_info, &buffer_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffer");
  };
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
//  vkFreeCommandBuffers(vcore_->get_device(), command_pool_->GetPool(), 1,
//                       &buffer_);
}

void VulkanCommandBuffer::AddRenderableObject(VulkanRenderableObject &object) {
  auto vertex_buffer = object.mesh_->VertexBuffer()->GetBuffer();
  auto vertex_offset = object.mesh_->VertexBuffer()->GetOffset();
  vkCmdBindVertexBuffers(buffer_, 0, 1, &vertex_buffer, &vertex_offset);
  auto index_buffer = object.mesh_->IndexBuffer()->GetBuffer();
  vkCmdBindIndexBuffer(buffer_, index_buffer, 0, VK_INDEX_TYPE_UINT16);
  auto pipeline_layout = object.pipeline_->GetPipelineLayout();
  vkCmdBindDescriptorSets(
      buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1,
      &object.descriptor_set_->GetDescriptor(), 0, nullptr);
  vkCmdDrawIndexed(buffer_, object.mesh_->IndexBuffer()->GetElementsNum(), 1, 0,
                   0, 0);
}

} // namespace slash