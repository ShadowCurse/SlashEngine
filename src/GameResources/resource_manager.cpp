#include "resource_manager.hpp"
#include "Renderer/renderer.hpp"
#include "slash_pch.hpp"
#include "object_info.hpp"

namespace slash {

void ResourceManager::Init() {
  if (instance_)
    return;
  instance_ = new ResourceManager();
  instance_->model_manager_ = new ModelManager();
  instance_->texture_manager_ = new TextureManager();
}

void ResourceManager::Destroy() {
  delete instance_->texture_manager_;
  delete instance_->model_manager_;
  delete instance_;
}

void ResourceManager::AddModel(Model model) {
  instance_->model_manager_->AddModel(model);
}

bool ResourceManager::LoadModel(const std::string &model_name, const std::string &model_path) {
  return instance_->model_manager_->LoadModel(model_name, model_path);
}

void ResourceManager::RemoveModel(const std::string &model_name) {
  instance_->model_manager_->RemoveModel(model_name);
}

void ResourceManager::BindModel(const std::string &model_name) {
  auto uid = instance_->model_manager_->GetUid(model_name);
  auto model = instance_->model_manager_->GetModel(model_name);
  Renderer::BindModel(uid, model.Vertices(), model.Indices());
}

void ResourceManager::UnBindModel(const std::string &model_name) {
  auto uid = instance_->model_manager_->GetUid(model_name);
  Renderer::UnBindModel(uid);
}

bool ResourceManager::LoadTexture(const std::string &texture_name, const std::string &texture_path) {
  return instance_->texture_manager_->LoadTexture(texture_name, texture_path);
}

void ResourceManager::DeleteTexture(const std::string &texture_name) {
  instance_->texture_manager_->DeleteTexture(texture_name);
}

void ResourceManager::BindTexture(const std::string &texture_name) {
  auto uid = instance_->texture_manager_->GetUid(texture_name);
  auto texture = instance_->texture_manager_->GetTexture(uid);
  Renderer::BindTexture(uid, texture);
}
void ResourceManager::UnBindTexture(const std::string &texture_name) {
  auto uid = instance_->texture_manager_->GetUid(texture_name);
  Renderer::UnBindTexture(uid);
}

void ResourceManager::BindObject(const std::string &model_name, const std::string &texture_name) {
  auto model_uid = instance_->model_manager_->GetUid(model_name);
  auto texture_uid = instance_->texture_manager_->GetUid(texture_name);
  ObjectInfo object_info;
  object_info.uid = std::hash<size_t>{}(model_uid + texture_uid);
  object_info.vertex_uid = model_uid;
  object_info.index_uid = model_uid;
  object_info.texture_uid = texture_uid;
  Renderer::BindObject(object_info);
}

} // namespace slash
