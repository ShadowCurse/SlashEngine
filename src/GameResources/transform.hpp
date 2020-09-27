#ifndef SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_

#include "Core/core.hpp"
#include "glm/glm.hpp"

namespace slash {

struct Slash_API Transform {
  glm::mat4 rotation;
  glm::vec3 position;
  glm::vec2 size;
};

}

#endif //SLASHENGINE_SRC_GAMERESOURCES_TRANSFORM_HPP_
