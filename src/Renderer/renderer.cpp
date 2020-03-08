#include "renderer.hpp"

namespace slash {

void Renderer::AddWindow(std::shared_ptr<Window> window) {
  api_instance_->AddWindow(std::move(window));
}
void Renderer::Init() { api_instance_->Init(); }

void Renderer::Destroy() { api_instance_->Destroy(); }

void Renderer::DrawFrame(float time) { api_instance_->DrawFrame(time); }

} // namespace slash
