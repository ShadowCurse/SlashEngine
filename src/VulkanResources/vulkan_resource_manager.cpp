#include "vulkan_resource_manager.hpp"
#include "Renderer/camera.hpp"
#include "VulkanRenderer/vulkan_renderer.hpp"

namespace slash {
VulkanResourceManager::VulkanResourceManager(VulkanRenderer *renderer)
    : renderer_(renderer) {
  CreateCameraBufer();
  CreateTextureSampler();
}

VulkanResourceManager::~VulkanResourceManager() {
  DeleteCameraBuffer();
  DeleteTextureSampler();
}

void VulkanResourceManager::CreateRenderableObject(Model *model) {
  auto descriptor_set = renderer_->CreateDescriptorSet();

  VkDescriptorBufferInfo buffer_info = {};
  buffer_info.buffer = camera_buffer_->GetBuffer();
  buffer_info.offset = 0;
  buffer_info.range = camera_buffer_->GetBufferSize();

  CreateRotationBuffer(model->uid_,
                       {model->rotation_, model->position_, model->size_});

  VkDescriptorBufferInfo rotation_info = {};
  rotation_info.buffer = rotation_buffers_[model->uid_]->GetBuffer();
  rotation_info.offset = 0;
  rotation_info.range = rotation_buffers_[model->uid_]->GetBufferSize();

  VkDescriptorImageInfo image_info = {};
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = texture_uids_[model->p_texture->uid_]->GetImageView();
  image_info.sampler = texture_sampler_->GetSampler();

  std::array<VkWriteDescriptorSet, 3> descriptor_writes = {};
  descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_writes[0].dstSet = descriptor_set->GetDescriptor();
  descriptor_writes[0].dstBinding = 0;
  descriptor_writes[0].dstArrayElement = 0;
  descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_writes[0].descriptorCount = 1;
  descriptor_writes[0].pBufferInfo = &buffer_info;
  descriptor_writes[0].pImageInfo = nullptr;
  descriptor_writes[0].pTexelBufferView = nullptr;

  descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_writes[1].dstSet = descriptor_set->GetDescriptor();
  descriptor_writes[1].dstBinding = 1;
  descriptor_writes[1].dstArrayElement = 0;
  descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_writes[1].descriptorCount = 1;
  descriptor_writes[1].pBufferInfo = &rotation_info;
  descriptor_writes[1].pImageInfo = nullptr;
  descriptor_writes[1].pTexelBufferView = nullptr;

  descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_writes[2].dstSet = descriptor_set->GetDescriptor();
  descriptor_writes[2].dstBinding = 2;
  descriptor_writes[2].dstArrayElement = 0;
  descriptor_writes[2].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptor_writes[2].descriptorCount = 1;
  descriptor_writes[2].pBufferInfo = nullptr;
  descriptor_writes[2].pImageInfo = &image_info;
  descriptor_writes[2].pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(renderer_->GetCore()->GetDevice(),
                         static_cast<uint32_t>(descriptor_writes.size()),
                         descriptor_writes.data(), 0, nullptr);

  VulkanRenderableObject object{};
  object.mesh_ = &mesh_uids_[model->p_mesh_->uid_];
  object.descriptor_set_ = descriptor_set;
  object.pipeline_ = renderer_->GetPipeline();
  renderable_objects_[model->uid_] = object;
}

void VulkanResourceManager::BindMesh(Mesh_3D *mesh) {
  VkDeviceSize index_buffer_size =
      sizeof(mesh->indices_[0]) * mesh->indices_.size();
  auto staging_index_buffer =
      VulkanStagingBuffer(renderer_->GetCore(), index_buffer_size);
  CopyMemory(mesh->indices_, &staging_index_buffer);
  auto index_buffer = new VulkanIndexBuffer(
      renderer_->GetCore(), index_buffer_size, mesh->indices_.size());
  CopyBuffer(&staging_index_buffer, index_buffer);

  VkDeviceSize vertex_buffer_size =
      sizeof(mesh->vertices_[0]) * mesh->vertices_.size();
  auto staging_vertex_buffer =
      VulkanStagingBuffer(renderer_->GetCore(), vertex_buffer_size);
  CopyMemory(mesh->vertices_, &staging_vertex_buffer);
  auto vertex_buffer =
      new VulkanVertexBuffer(renderer_->GetCore(), vertex_buffer_size, 0);
  CopyBuffer(&staging_vertex_buffer, vertex_buffer);

  mesh_uids_[mesh->uid_] = VulkanMesh{vertex_buffer, index_buffer};
}

void VulkanResourceManager::UnBindMesh(Mesh_3D *mesh) {
  auto stored_mesh = mesh_uids_.find(mesh->uid_);
  if (stored_mesh == mesh_uids_.end())
    return;
  delete stored_mesh->second.vertex_buffer;
  delete stored_mesh->second.index_buffer;
  mesh_uids_.erase(stored_mesh);
}

void VulkanResourceManager::BindTexture(Texture *texture) {
  VkDeviceSize image_size = texture->width_ * texture->height_ * 4;
  auto staging_buffer = VulkanStagingBuffer(renderer_->GetCore(), image_size);
  CopyMemory(texture->pixels_, &staging_buffer);

  auto texture_image = new VulkanTexture(
      renderer_->GetCore(), texture->width_, texture->height_,
      VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  TransitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToTexture(&staging_buffer, texture_image);
  TransitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  texture_uids_[texture->uid_] = texture_image;
}

void VulkanResourceManager::UnBindTexture(Texture *texture) {
  auto stored_texture = texture_uids_.find(texture->uid_);
  if (stored_texture == texture_uids_.end())
    return;
  delete stored_texture->second;
  texture_uids_.erase(stored_texture);
}

void VulkanResourceManager::CreateRotationBuffer(size_t uid,
                                                 Transform transform) {
  VkDeviceSize buffer_size = sizeof(Transform);
  auto rotation_buffer = new VulkanBuffer(
      renderer_->GetCore(), buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  void *ptr;
  vkMapMemory(renderer_->GetCore()->GetDevice(), rotation_buffer->GetMemory(),
              0, buffer_size, 0, &ptr);
  memcpy(ptr, &transform, static_cast<size_t>(buffer_size));
  vkUnmapMemory(renderer_->GetCore()->GetDevice(),
                rotation_buffer->GetMemory());
  rotation_buffers_[uid] = rotation_buffer;
}

void VulkanResourceManager::DeleteRotationBuffer(size_t uid) {
  auto stored_buffer = rotation_buffers_.find(uid);
  if (stored_buffer == rotation_buffers_.end())
    return;
  delete stored_buffer->second;
  rotation_buffers_.erase(stored_buffer);
}

void VulkanResourceManager::CopyBuffer(VulkanBuffer *src, VulkanBuffer *dst) {
  auto command_buffer = renderer_->BeginOneTimeCommand();
  VkBufferCopy copyRegion = {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = src->GetBufferSize();
  vkCmdCopyBuffer(command_buffer->GetBuffer(), src->GetBuffer(),
                  dst->GetBuffer(), 1, &copyRegion);
  renderer_->EndOneTimeCommand(command_buffer);
}

template <typename T>
void VulkanResourceManager::CopyMemory(const std::vector<T> &data,
                                       VulkanBuffer *buffer) {
  VkDeviceSize size = sizeof(T) * data.size();
  void *ptr;
  vkMapMemory(renderer_->GetCore()->GetDevice(), buffer->GetMemory(), 0, size,
              0, &ptr);
  memcpy(ptr, data.data(), static_cast<size_t>(size));
  vkUnmapMemory(renderer_->GetCore()->GetDevice(), buffer->GetMemory());
}

void VulkanResourceManager::CopyBufferToTexture(VulkanBuffer *buffer,
                                                VulkanTexture *texture) {
  auto command_buffer = renderer_->BeginOneTimeCommand();
  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {texture->Width(), texture->Height(), 1};
  vkCmdCopyBufferToImage(command_buffer->GetBuffer(), buffer->GetBuffer(),
                         texture->GetImage(),
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  renderer_->EndOneTimeCommand(command_buffer);
}

void VulkanResourceManager::TransitionImageLayout(VulkanTexture *texture,
                                                  VkFormat format,
                                                  VkImageLayout oldLayout,
                                                  VkImageLayout newLayout) {
  auto command_buffer = renderer_->BeginOneTimeCommand();
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = texture->GetImage();
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
        format == VK_FORMAT_D24_UNORM_S8_UINT) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }
  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;
  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::runtime_error("unsupported layout transition");
  }
  vkCmdPipelineBarrier(command_buffer->GetBuffer(), source_stage,
                       destination_stage, 0, 0, nullptr, 0, nullptr, 1,
                       &barrier);
  renderer_->EndOneTimeCommand(command_buffer);
}
void VulkanResourceManager::DestroyRenderableObject(Model *model) {
  renderable_objects_.erase(model->uid_);
  DeleteRotationBuffer(model->uid_);
}

void VulkanResourceManager::CreateCameraBufer() {
  Camera camera;
  camera.SetRatio(static_cast<float>(16) / 9);
  VkDeviceSize buffer_size = sizeof(Camera);
  camera_buffer_ = std::make_shared<VulkanBuffer>(
      renderer_->GetCore(), buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  void *ptr;
  vkMapMemory(renderer_->GetCore()->GetDevice(), camera_buffer_->GetMemory(), 0,
              buffer_size, 0, &ptr);
  memcpy(ptr, &camera, static_cast<size_t>(buffer_size));
  vkUnmapMemory(renderer_->GetCore()->GetDevice(), camera_buffer_->GetMemory());
}

void VulkanResourceManager::DeleteCameraBuffer() {}

void VulkanResourceManager::CreateTextureSampler() {
  texture_sampler_ =
      std::make_shared<VulkanTextureSampler>(renderer_->GetCore());
}

void VulkanResourceManager::DeleteTextureSampler() {}

VulkanMesh VulkanResourceManager::GetMesh(size_t uid) {
  return mesh_uids_[uid];
}

VulkanTexture *VulkanResourceManager::GetTexture(size_t uid) {
  return texture_uids_[uid];
}

VulkanBuffer *VulkanResourceManager::GetRotationBuffer(size_t uid) {
  return rotation_buffers_[uid];
}

VulkanBuffer *VulkanResourceManager::GetCameraBuffer() {
  return camera_buffer_.get();
}

VulkanTextureSampler *VulkanResourceManager::GetTextureSampler() {
  return texture_sampler_.get();
}

std::vector<VulkanRenderableObject *>
VulkanResourceManager::GetRenderableObjects() {
  std::vector<VulkanRenderableObject *> ret;
  for (auto &pair : renderable_objects_) {
    ret.push_back(&pair.second);
  }
  return ret;
}

} // namespace slash
