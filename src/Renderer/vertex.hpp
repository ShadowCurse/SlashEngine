#ifndef SLASHENGINE_VERTEX_H_
#define SLASHENGINE_VERTEX_H_

#include "Core/core.hpp"
#include "glm/glm.hpp"

namespace slash {

struct Slash_API Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;
};

struct Slash_API UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

} // namespace slash

#endif // SLASHENGINE_VERTEX_H_