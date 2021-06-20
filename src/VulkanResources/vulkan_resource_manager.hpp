#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_

#include "Core/app.hpp"
#include "Core/events.hpp"
#include "Ecs/entity.hpp"
#include "vulkan_mesh.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_texture.hpp"

namespace slash {

class VulkanResourceManager {
 public:
  explicit VulkanResourceManager(App *app, class VulkanRenderer *renderer);

  void create_renderable_object(Entity entity);

  auto create_mesh(Entity entity) -> std::unique_ptr<VulkanMesh>;
  auto create_texture(Entity entity) -> std::unique_ptr<VulkanTexture>;
  auto create_transform(Entity entity) -> std::unique_ptr<VulkanBuffer>;
  void update_transform(Entity entity);

  void create_camera_buffer();
  void create_texture_sampler();

 private:
  void copy_buffer(VulkanBuffer *src, VulkanBuffer *dst);
  void copy_buffer_to_texture(VulkanBuffer *buffer, VulkanTexture *texture);
  template<typename T>
  void copy_memory(const std::vector<T> &data, VulkanBuffer *buffer);
  void transition_image_layout(VulkanTexture *texture, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout);

 private:
  App *app_;
  VulkanRenderer *renderer_;

  std::shared_ptr<VulkanBuffer> camera_buffer_;
  std::shared_ptr<VulkanTextureSampler> texture_sampler_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_
