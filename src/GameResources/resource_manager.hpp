#ifndef SLASHENGINE_SRC_GAMERESOURCES_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_RESOURCE_MANAGER_HPP_

#include "Core/core.hpp"
#include "mesh_manager.hpp"
#include "model_manager.hpp"
#include "texture_manager.hpp"
#include <VulkanResources/vulkan_renderable_object.hpp>

namespace slash {

class Slash_API ResourceManager {
public:
  static void Init();
  static void Destroy();

  static void CreateRenderableObject(const std::string& object_name);

  static void AddModel(const std::string &name, const std::string &mesh_name,
                       const std::string &texture_name,
                       const glm::mat4 &rotation, const glm::vec3 &position,
                       const glm::vec2 &size);

  static std::shared_ptr<Model> GetModel(const std::string &name);

  static void RemoveModel(const std::string &name);

  static void AddMesh(const std::string &name,
                      const std::vector<Vertex> &vertices,
                      const std::vector<uint16_t> &indices);
  static void RemoveMesh(const std::string &name);

  static bool LoadTexture(const std::string &texture_name,
                          const std::string &texture_path);
  static void RemoveTexture(const std::string &name);

  static void AddDependencies(std::shared_ptr<Model> model);

  static void RemoveDependencies(std::shared_ptr<Model> model);
private:

  ModelManager *model_manager_;
  MeshManager *mesh_manager_;
  TextureManager *texture_manager_;

  std::vector<std::shared_ptr<Model>> model_binded_;
  std::map<std::shared_ptr<Texture>, std::vector<std::shared_ptr<Model>>>
      texture_usages;
  std::map<std::shared_ptr<Mesh_3D>, std::vector<std::shared_ptr<Model>>>
      meshes_usages;

  inline static ResourceManager *instance_ = nullptr;
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_
