#ifndef SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_

#include "Core/core.hpp"
#include "Renderer/vertex.hpp"

namespace slash {

struct Mesh {
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
};

struct Triangle {
  [[nodiscard]] static auto create() -> Mesh {
    return Mesh{{
                    {{-1.0f, -1.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{1.0f, -1.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}},
                {0, 1, 2}};
  }
};

struct Square {
  [[nodiscard]] static auto create() -> Mesh {
    return Mesh{

        {{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
         {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
         {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
         {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}},

        {0, 1, 2, 2, 3, 0}};
  }
};

struct Cube {
  [[nodiscard]] static auto create() -> Mesh {
    return Mesh{{
                    {{-0.5f, -0.5f, 1.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{0.5f, -0.5f, 1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{0.5f, 0.5f, 1.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                    {{-0.5f, 0.5f, 1.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

                    {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                    {{-0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}},

                {0, 1, 2, 2, 3, 0,
                 0, 3, 7, 7, 4, 0,
                 0, 1, 5, 5, 4, 0,
                 2, 1, 5, 5, 2, 6,
                 3, 2, 6, 6, 7, 3,
                 4, 5, 6, 6, 7, 4}};
  }
};

} // namespace slash

#endif // SLASHENGINE_SRC_GAMERESOURCES_MESH_3D_HPP_
