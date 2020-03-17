#include "resource_manager.hpp"
#include "Renderer/renderer.hpp"
#include "slash_pch.hpp"

namespace slash {

void ResourceManager::Init() {
  if (instance_)
    return;
  instance_ = new ResourceManager();
  instance_->model_manager_ = new ModelManager();
}

void ResourceManager::Destroy() {
  delete instance_->model_manager_;
  delete instance_;
}

ModelManager *ResourceManager::GetModelManager() {
  return instance_->model_manager_;
}

} // namespace slash
