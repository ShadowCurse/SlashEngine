#include "model_manager.hpp"
#include "Renderer/renderer.hpp"

namespace slash {

void ModelManager::AddModel(Model model) {
  models_.insert({std::hash<std::string>{}(model.name), std::move(model)});
}

void ModelManager::RemoveModel(const std::string &model_name) {
  auto uid = std::hash<std::string>{}(model_name);
  models_.erase(uid);
}

void ModelManager::BindModel(const std::string &model_name) {
  auto uid = std::hash<std::string>{}(model_name);
  if (models_[uid].binded)
    return;
  Renderer::BindVertexBuffer(uid, models_[uid].vertices);
  Renderer::BindIndexBuffer(uid, models_[uid].indices);
  models_[uid].binded = true;
}

void ModelManager::UnBindModel(const std::string &model_name) {
  auto uid = std::hash<std::string>{}(model_name);
  if (!models_[uid].binded)
    return;
  Renderer::UnBindVertexBuffer(uid);
  Renderer::UnBindIndexBuffer(uid);
  models_[uid].binded = false;
}

} // namespace slash