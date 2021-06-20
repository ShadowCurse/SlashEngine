#ifndef SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/mat4x4.hpp>

#include "Core/core.hpp"

namespace slash {

struct Slash_API Transform {
  glm::mat4 translation;
  glm::mat4 rotation;
  glm::mat4 scale;
};

}

#endif //SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_
