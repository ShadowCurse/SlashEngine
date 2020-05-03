#ifndef SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_

#include "Core/core.hpp"
#include "Renderer/vertex.hpp"

namespace slash {

class Mesh_3D {
public:
  Mesh_3D(std::string name, size_t uid, std::vector<Vertex> vertices,
          std::vector<uint16_t> indices)
      : name_(std::move(name)), uid_(uid), vertices_(std::move(vertices)),
        indices_(std::move(indices)) {}
  std::string name_;
  size_t uid_;
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_
