#ifndef SLASHENGINE_SRC_GAMERESOURCES_PACK_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_PACK_HPP_

#include "mesh.hpp"
#include "transform.hpp"
#include "texture.hpp"

namespace slash {

struct PackObject3d {
  Mesh mesh;
  Transform transform;
  Texture texture;
};

}

#endif //SLASHENGINE_SRC_GAMERESOURCES_PACK_HPP_
