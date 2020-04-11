#include "renderer.hpp"

namespace slash {

void Renderer::AddWindow(std::shared_ptr<Window> window) {
  api_instance_->AddWindow(std::move(window));
}
void Renderer::Init() { api_instance_->Init(); }

void Renderer::Destroy() { api_instance_->Destroy(); }

void Renderer::DrawFrame(float time) { api_instance_->DrawFrame(time); }

void Renderer::UpdateScene() { api_instance_->UpdateScene(); }

void Renderer::BindVertexBuffer(size_t uid,
                                const std::vector<Vertex> &vertices) {
  api_instance_->BindVertexBuffer(uid, vertices);
}

void Renderer::BindIndexBuffer(size_t uid,
                               const std::vector<uint16_t> &indices) {
  api_instance_->BindIndexBuffer(uid, indices);
}

void Renderer::UnBindVertexBuffer(size_t uid) {
  api_instance_->UnBindVertexBuffer(uid);
}

void Renderer::UnBindIndexBuffer(size_t uid) {
  api_instance_->UnBindIndexBuffer(uid);
}

void Renderer::BindTexture(size_t uid, const Texture &texture) {
  api_instance_->BindTexture(uid, texture);
}

void Renderer::UnBindTexture(size_t uid) {
  api_instance_->UnBindTexture(uid);
}

void Renderer::BindObject(const ObjectInfo &object_info) {
  api_instance_->BindObject(object_info);
}

} // namespace slash
