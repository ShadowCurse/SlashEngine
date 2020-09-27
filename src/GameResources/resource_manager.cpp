#include "resource_manager.hpp"
#include "Scene/scene_manager.hpp"
#include "transform.hpp"
#include "mesh_3d.hpp"
#include "texture.hpp"

namespace slash {

void ResourceManager::Init() {
  SceneManager::GetScene().GetECS().RegisterComponent<Mesh_3D>();
  SceneManager::GetScene().GetECS().RegisterComponent<Texture>();
  SceneManager::GetScene().GetECS().RegisterComponent<Transform>();
}

void ResourceManager::Destroy() {
}

} // namespace slash
