#ifndef SL_VERTEX
#define SL_VERTEX

#include "glm/glm.hpp"

namespace Slash
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
} // namespace Slash

#endif