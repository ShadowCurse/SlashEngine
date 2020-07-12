#ifndef SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_

#include "GameResources/mesh_3d.hpp"
#include "GameResources/model.hpp"
#include "GameResources/texture.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_index_buffer.hpp"
#include "vulkan_mesh.hpp"
#include "vulkan_renderable_object.hpp"
#include "vulkan_texture.hpp"
#include "vulkan_vertex_buffer.hpp"

namespace slash {

struct Transform {
  glm::mat4 rotation;
  glm::vec3 position;
  glm::vec2 size;
};

class VulkanResourceManager {
public:
  explicit VulkanResourceManager(class VulkanRenderer *renderer);
  ~VulkanResourceManager();

  void CreateRenderableObject(Model *model);
  void DestroyRenderableObject(Model *model);

  void BindMesh(Mesh_3D *mesh);
  void UnBindMesh(Mesh_3D *mesh);
  void BindTexture(Texture *texture);
  void UnBindTexture(Texture *texture);
  void CreateRotationBuffer(size_t uid, Transform transform);
  void DeleteRotationBuffer(size_t uid);
  void CreateCameraBufer();
  void DeleteCameraBuffer();
  void CreateTextureSampler();
  void DeleteTextureSampler();

  [[nodiscard]] VulkanMesh GetMesh(size_t uid);
  [[nodiscard]] VulkanTexture *GetTexture(size_t uid);
  [[nodiscard]] VulkanBuffer *GetRotationBuffer(size_t uid);
  [[nodiscard]] VulkanBuffer *GetCameraBuffer();
  [[nodiscard]] VulkanTextureSampler *GetTextureSampler();
  [[nodiscard]] std::vector<VulkanRenderableObject *> GetRenderableObjects();

private:
  void CopyBuffer(VulkanBuffer *src, VulkanBuffer *dst);
  void CopyBufferToTexture(VulkanBuffer *buffer, VulkanTexture *texture);
  template <typename T>
  void CopyMemory(const std::vector<T> &data, VulkanBuffer *buffer);
  void TransitionImageLayout(VulkanTexture *texture, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  VulkanRenderer *renderer_;

  std::map<size_t, VulkanMesh> mesh_uids_;
  std::map<size_t, VulkanTexture *> texture_uids_;
  std::map<size_t, VulkanBuffer *> rotation_buffers_;
  std::map<size_t, VulkanRenderableObject> renderable_objects_;
  std::shared_ptr<VulkanBuffer> camera_buffer_;
  std::shared_ptr<VulkanTextureSampler> texture_sampler_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_VULKANRESOURCES_VULKAN_RESOURCE_MANAGER_HPP_
