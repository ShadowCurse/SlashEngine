#include "model_manager.hpp"

namespace slash {

Model::Model(const std::string &name, const std::vector<Vertex> &vertices,
             const std::vector<uint16_t> &indices)
    : name_(name), vertices_(vertices), indices_(indices) {}

const std::string &Model::Name() const {
  return name_;
}

const std::vector<Vertex> &Model::Vertices() const {
  return vertices_;
}

const std::vector<uint16_t> &Model::Indices() const {
  return indices_;
}

bool ModelManager::LoadModel([[maybe_unused]] const std::string &model_name,
                             [[maybe_unused]] const std::string &model_path) { return false; }

void ModelManager::AddModel(Model model) {
  auto uid = std::hash<std::string>{}(model.name_);
  models_.insert({uid, std::move(model)});
}

void ModelManager::RemoveModel(const std::string &model_name) {
  auto uid = std::hash<std::string>{}(model_name);
  models_.erase(uid);
}
size_t ModelManager::GetUid(const std::string &model_name) const {
  return std::hash<std::string>{}(model_name);
}
const Model &ModelManager::GetModel(const std::string &model_name) const {
  auto uid = std::hash<std::string>{}(model_name);
  return models_.at(uid);
}

} // namespace slash