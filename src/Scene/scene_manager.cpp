#include "scene_manager.hpp"
#include "GameResources/resource_manager.hpp"

namespace slash {

void SceneManager::PrepareScene() {
  auto render_command = RenderModule::Renderer()->StartRenderCommand();
  for (auto &object : RenderModule::ResourceManager()->GetRenderableObjects())
    render_command->AddRenderableObject(object);
  RenderModule::Renderer()->EndRenderCommand(render_command);
}

void SceneManager::DrawScene(double dt) {
  RenderModule::Renderer()->DrawFrame(dt);
}

void SceneManager::AddToScene(const std::string &object_name) {
  auto model = ResourceManager::GetModel(object_name);
  ResourceManager::AddDependencies(model);
  ResourceManager::CreateRenderableObject(object_name);
}

} // namespace slash
