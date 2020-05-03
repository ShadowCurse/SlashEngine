#ifndef SLASHENGINE_SRC_GAMERESOURCES_MODEL_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_MODEL_HPP_

#include "Core/core.hpp"
#include "glm/glm.hpp"
#include "mesh_3d.hpp"
#include "texture.hpp"

namespace slash {

class Slash_API Model {
public:
  Model(std::string name, size_t uid, std::shared_ptr<Mesh_3D> mesh,
        std::shared_ptr<Texture> texture, const glm::mat4 &rotation,
        const glm::vec3 &position, const glm::vec2 &size_)
      : name_(std::move(name)), uid_(uid), p_mesh_(std::move(mesh)),
        p_texture(std::move(texture)), rotation_(rotation), position_(position),
        size_(size_) {}

  std::string name_;
  size_t uid_;
  std::shared_ptr<Mesh_3D> p_mesh_;
  std::shared_ptr<Texture> p_texture;
  glm::mat4 rotation_;
  glm::vec3 position_;
  glm::vec2 size_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_GAMERESOURCES_MODEL_HPP_
