#include "scene_manager.hpp"
#include "GameResources/resource_manager.hpp"
#include "RendererModule/render_module.hpp"

namespace slash {

void SceneManager::Init() {
  scene_ = new Scene();
}

void SceneManager::Destroy() {
  delete scene_;
}

void SceneManager::PrepareScene() {
  RenderModule::Renderer().NewFrame();
  auto render_command = RenderModule::Renderer().StartRenderCommand();
  for (auto &object: *SceneManager::GetScene().GetECS().GetComponentArray<VulkanRenderableObject>())
    render_command->AddRenderableObject(object);
  RenderModule::Renderer().EndRenderCommand(render_command);
}

void SceneManager::DrawScene(double dt) {
  RenderModule::Renderer().DrawFrame(dt);
}
void SceneManager::AddToScene(const Entity entity) {
  RenderModule::ResourceManager().CreateRenderableObject(entity);
}

Scene &SceneManager::GetScene() { return *scene_; }

} // namespace slash
