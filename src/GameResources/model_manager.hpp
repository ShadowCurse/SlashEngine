#ifndef SLASHENGINE_SRC_GAMERESOURCES_MODEL_MANAGER_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_MODEL_MANAGER_HPP_

#include "Core/core.hpp"
#include "Renderer/vertex.hpp"
#include "model.hpp"

namespace slash {

class Slash_API ModelManager {
public:
  void AddModel(const std::string &model_name, std::shared_ptr<Mesh_3D> mesh,
                std::shared_ptr<Texture> texture, const glm::mat4 &rotation,
                const glm::vec3 &position, const glm::vec2 &size);
  void RemoveModel(const std::string &model_name);
  [[nodiscard]] std::shared_ptr<Model> GetModel(const std::string &model_name);

private:
  [[nodiscard]] size_t GenUid(const std::string &model_name) const;
  std::map<size_t, std::shared_ptr<Model>> models_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_
