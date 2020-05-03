#include "mesh_manager.hpp"
#include "Core/log.hpp"

void slash::MeshManager::AddMesh(const std::string &mesh_name,
                                 const std::vector<Vertex> &vertices,
                                 const std::vector<uint16_t> &indices) {
  auto uid = GenUid(mesh_name);
  if (meshes_.find(uid) != meshes_.end()) {
    SL_CORE_ERROR(
        "Trying to add mesh with name {}, but the name is already used",
        mesh_name);
    return;
  }
  meshes_.insert(
      {uid, std::make_shared<Mesh_3D>(mesh_name, uid, vertices, indices)});
}

void slash::MeshManager::RemoveMesh(const std::string &mesh_name) {
  auto uid = GenUid(mesh_name);
  if (meshes_.find(uid) == meshes_.end()) {
    SL_CORE_ERROR("Trying to remove mesh with name {}, but it does not exist",
                  mesh_name);
    return;
  }
  meshes_.erase(uid);
}

std::shared_ptr<slash::Mesh_3D>
slash::MeshManager::GetMesh(const std::string &mesh_name) {
  auto uid = GenUid(mesh_name);
  if (meshes_.find(uid) == meshes_.end()) {
    SL_CORE_ERROR("No mesh with name {0} exist", mesh_name);
    return {};
  }
  return meshes_[uid];
}

size_t slash::MeshManager::GenUid(const std::string &mesh_name) const {
  return std::hash<std::string>{}("Mesh_3D" + mesh_name);
}
