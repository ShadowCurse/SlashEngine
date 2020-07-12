#include "resource_manager.hpp"
#include "Core/log.hpp"
#include "RendererModule/render_module.hpp"

namespace slash {

void ResourceManager::Init() {
  if (instance_)
    return;
  instance_ = new ResourceManager();
  instance_->model_manager_ = new ModelManager();
  instance_->mesh_manager_ = new MeshManager();
  instance_->texture_manager_ = new TextureManager();
}

void ResourceManager::Destroy() {
  delete instance_->texture_manager_;
  delete instance_->mesh_manager_;
  delete instance_->model_manager_;
  delete instance_;
}

void ResourceManager::CreateRenderableObject(const std::string &object_name) {
  RenderModule::ResourceManager()->CreateRenderableObject(
      instance_->model_manager_->GetModel(object_name).get());
}

void ResourceManager::AddModel(const std::string &name,
                               const std::string &mesh_name,
                               const std::string &texture_name,
                               const glm::mat4 &rotation,
                               const glm::vec3 &position,
                               const glm::vec2 &size) {
  instance_->model_manager_->AddModel(
      name, instance_->mesh_manager_->GetMesh(mesh_name),
      instance_->texture_manager_->GetTexture(texture_name), rotation, position,
      size);
}

std::shared_ptr<Model> ResourceManager::GetModel(const std::string &name) {
  return instance_->model_manager_->GetModel(name);
}

void ResourceManager::RemoveModel(const std::string &name) {
  instance_->model_manager_->RemoveModel(name);
}

// void ResourceManager::BindModel(const std::string &name) {
//  auto model = instance_->model_manager_->GetModel(name);
//  if (std::find(instance_->model_binded_.begin(),
//                instance_->model_binded_.end(),
//                model) != instance_->model_binded_.end()) {
//    SL_CORE_WARN("Model {} already binded", model->name_);
//    return;
//  }
//  instance_->AddDependencies(model);
//  RenderModule::ResourceManager()->BindModel(model.get());
//  instance_->model_binded_.emplace_back(model);
//}
//
// void ResourceManager::UnBindModel(const std::string &name) {
//  auto model = instance_->model_manager_->GetModel(name);
//  auto iter = std::find(instance_->model_binded_.begin(),
//                        instance_->model_binded_.end(), model);
//  if (iter == instance_->model_binded_.end()) {
//    SL_CORE_WARN("Model {} not binded", model->name_);
//    return;
//  }
//  RenderModule::ResourceManager()->UnBindModel(model.get());
//  instance_->RemoveDependencies(model);
//  instance_->model_binded_.erase(iter);
//}

void ResourceManager::AddMesh(const std::string &name,
                              const std::vector<Vertex> &vertices,
                              const std::vector<uint16_t> &indices) {
  instance_->mesh_manager_->AddMesh(name, vertices, indices);
}

void ResourceManager::RemoveMesh(const std::string &name) {
  instance_->mesh_manager_->RemoveMesh(name);
}

bool ResourceManager::LoadTexture(const std::string &texture_name,
                                  const std::string &texture_path) {
  return instance_->texture_manager_->LoadTexture(texture_name, texture_path);
}

void ResourceManager::RemoveTexture(const std::string &name) {
  instance_->texture_manager_->RemoveTexture(name);
}

void ResourceManager::AddDependencies(std::shared_ptr<Model> model) {
  if (instance_->texture_usages.find(model->p_texture) ==
      instance_->texture_usages.end()) {
    RenderModule::ResourceManager()->BindTexture(model->p_texture.get());
    instance_->texture_usages.insert({model->p_texture, {model}});
  } else {
    instance_->texture_usages[model->p_texture].push_back(model);
  }
  if (instance_->meshes_usages.find(model->p_mesh_) ==
      instance_->meshes_usages.end()) {
    RenderModule::ResourceManager()->BindMesh(model->p_mesh_.get());
    instance_->meshes_usages.insert({model->p_mesh_, {model}});
  } else {
    instance_->meshes_usages[model->p_mesh_].push_back(model);
  }
}

void ResourceManager::RemoveDependencies(std::shared_ptr<Model> model) {
  if (instance_->texture_usages.find(model->p_texture) ==
      instance_->texture_usages.end()) {
    SL_CORE_WARN("While removing model {} texture {} was already removed",
                 model->name_, model->p_texture->name_);
  } else {
    auto usages = instance_->texture_usages[model->p_texture];
    usages.erase(std::find(usages.begin(), usages.end(), model));
    if (usages.empty()) {
      RenderModule::ResourceManager()->UnBindTexture(model->p_texture.get());
      instance_->texture_usages.erase(model->p_texture);
    }
  }
  if (instance_->meshes_usages.find(model->p_mesh_) ==
      instance_->meshes_usages.end()) {
    SL_CORE_WARN("While removing model {} mesh {} was already removed",
                 model->name_, model->p_mesh_->name_);
  } else {
    auto usages = instance_->meshes_usages[model->p_mesh_];
    usages.erase(std::find(usages.begin(), usages.end(), model));
    if (usages.empty()) {
      RenderModule::ResourceManager()->UnBindMesh(model->p_mesh_.get());
      instance_->meshes_usages.erase(model->p_mesh_);
    }
  }
}

} // namespace slash
