#include "renderer.hpp"

namespace slash {

void Renderer::AddWindow(std::shared_ptr<Window> window) {
  api_instance_->AddWindow(std::move(window));
}
void Renderer::Init() { api_instance_->Init(); }

void Renderer::Destroy() { api_instance_->Destroy(); }

void Renderer::DrawFrame(double time) { api_instance_->DrawFrame(time); }

void Renderer::UpdateScene() { api_instance_->UpdateScene(); }

void Renderer::BindModel(std::shared_ptr<Model> model) {
  api_instance_->BindModel(std::move(model));
}

void Renderer::UnBindModel(std::shared_ptr<Model> model) {
  api_instance_->UnBindModel(std::move(model));
}

void Renderer::BindMesh(std::shared_ptr<Mesh_3D> mesh) {
  api_instance_->BindMesh(std::move(mesh));
}

void Renderer::UnBindMesh(std::shared_ptr<Mesh_3D> mesh) {
  api_instance_->UnBindMesh(std::move(mesh));
}

void Renderer::BindTexture(std::shared_ptr<Texture> texture) {
  api_instance_->BindTexture(std::move(texture));
}

void Renderer::UnBindTexture(std::shared_ptr<Texture> texture) {
  api_instance_->UnBindTexture(std::move(texture));
}

} // namespace slash
