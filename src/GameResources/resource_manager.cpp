#include "resource_manager.hpp"
#include "Scene/scene_manager.hpp"
#include "transform.hpp"
#include "mesh_3d.hpp"
#include "texture.hpp"

namespace slash {

void ResourceManager::Init() {
  SceneManager::GetScene().GetECS().register_component<Mesh_3D>();
  SceneManager::GetScene().GetECS().register_component<Texture>();
  SceneManager::GetScene().GetECS().register_component<Transform>();
}

void ResourceManager::Destroy() {
}

} // namespace slash
