#ifndef SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Core/core.hpp"

namespace slash {

struct Slash_API Transform {
  glm::vec3 translation;
  glm::mat4 rotation;
  glm::vec3 scale;
};

}

#endif //SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_
