#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_

#include "GameResources/mesh_3d.hpp"
#include "GameResources/transform.hpp"
#include "GameResources/texture.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_index_buffer.hpp"
#include "vulkan_mesh.hpp"
#include "vulkan_renderable_object.hpp"
#include "vulkan_texture.hpp"
#include "vulkan_vertex_buffer.hpp"
#include "Scene/scene_manager.hpp"

namespace slash {

class VulkanResourceManager {
 public:
  explicit VulkanResourceManager(class VulkanRenderer *renderer);
  ~VulkanResourceManager();

  void CreateRenderableObject(Entity entity);
//  void DestroyRenderableObject(Model *model);

  auto CreateMesh(Entity entity) -> std::shared_ptr<VulkanMesh>;
  auto CreateTexture(Entity entity) -> std::shared_ptr<VulkanTexture>;
  auto CreateTransform(Entity entity) -> std::shared_ptr<VulkanBuffer>;
  void CreateCameraBuffer();
  void DeleteCameraBuffer();
  void CreateTextureSampler();
  void DeleteTextureSampler();

//  [[nodiscard]] VulkanMesh GetMesh(size_t uid);
//  [[nodiscard]] VulkanTexture *GetTexture(size_t uid);
//  [[nodiscard]] VulkanBuffer *GetRotationBuffer(size_t uid);
//  [[nodiscard]] VulkanBuffer *GetCameraBuffer();
//  [[nodiscard]] VulkanTextureSampler *GetTextureSampler();
//  [[nodiscard]] std::vector<VulkanRenderableObject *> GetRenderableObjects();

 private:
  void CopyBuffer(VulkanBuffer *src, VulkanBuffer *dst);
  void CopyBufferToTexture(VulkanBuffer *buffer, VulkanTexture *texture);
  template<typename T>
  void CopyMemory(const std::vector<T> &data, VulkanBuffer *buffer);
  void TransitionImageLayout(VulkanTexture *texture, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  VulkanRenderer *renderer_;

//  std::map<size_t, VulkanMesh> mesh_uids_;
//  std::map<size_t, VulkanTexture *> texture_uids_;
//  std::map<size_t, VulkanBuffer *> rotation_buffers_;
//  std::map<size_t, VulkanRenderableObject> renderable_objects_;
  std::shared_ptr<VulkanBuffer> camera_buffer_;
  std::shared_ptr<VulkanTextureSampler> texture_sampler_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_
