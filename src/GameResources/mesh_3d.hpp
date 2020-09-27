#ifndef SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_

#include "Core/core.hpp"
#include "Renderer/vertex.hpp"

namespace slash {

struct Mesh_3D {
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_
