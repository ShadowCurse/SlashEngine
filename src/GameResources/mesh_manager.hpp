#ifndef SLASHENGINE_SRC_GAMERESOURCES_MESH_MANAGER_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_MESH_MANAGER_HPP_

#include "Core/core.hpp"
#include "mesh_3d.hpp"

namespace slash {

class MeshManager {
public:
  void AddMesh(const std::string &mesh_name,
               const std::vector<Vertex> &vertices,
               const std::vector<uint16_t> &indices);
  void RemoveMesh(const std::string &mesh_name);
  [[nodiscard]] std::shared_ptr<Mesh_3D> GetMesh(const std::string &mesh_name);

private:
  [[nodiscard]] size_t GenUid(const std::string &mesh_name) const;
  std::map<size_t, std::shared_ptr<Mesh_3D>> meshes_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_GAMERESOURCES_MESH_MANAGER_HPP_
