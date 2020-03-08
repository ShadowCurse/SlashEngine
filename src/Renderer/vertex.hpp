#ifndef SLASHENGINE_VERTEX_H_
#define SLASHENGINE_VERTEX_H_

#include "glm/glm.hpp"

namespace slash {

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};
} // namespace slash

#endif // SLASHENGINE_VERTEX_H_