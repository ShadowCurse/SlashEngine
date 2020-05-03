#include "model_manager.hpp"
#include "Core/log.hpp"

namespace slash {

void ModelManager::AddModel(const std::string &model_name,
                            std::shared_ptr<Mesh_3D> mesh,
                            std::shared_ptr<Texture> texture,
                            const glm::mat4 &rotation,
                            const glm::vec3 &position, const glm::vec2 &size) {
  auto uid = GenUid(model_name);
  if (models_.find(uid) != models_.end()) {
    SL_CORE_ERROR(
        "Trying to add model with name {}, but the name is already used",
        model_name);
    return;
  }
  models_.insert({uid, std::make_shared<Model>(model_name, uid, mesh, texture,
                                               rotation, position, size)});
}

void ModelManager::RemoveModel(const std::string &model_name) {
  auto uid = GenUid(model_name);
  if (models_.find(uid) == models_.end()) {
    SL_CORE_ERROR("Trying to remove model with name {}, but it does not exist",
                  model_name);
    return;
  }
  models_.erase(uid);
}

std::shared_ptr<Model> ModelManager::GetModel(const std::string &model_name) {
  auto uid = GenUid(model_name);
  if (models_.find(uid) == models_.end()) {
    SL_CORE_ERROR("No model with name {0} exist", model_name);
    return {};
  }
  return models_.at(uid);
}

size_t ModelManager::GenUid(const std::string &model_name) const {
  return std::hash<std::string>{}("Model" + model_name);
}

} // namespace slash