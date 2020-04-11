#include "application.hpp"
#include "GameResources/resource_manager.hpp"
#include "Renderer/renderer.hpp"
#include "VulkanRenderer/vulkan_rendererAPI.hpp"
#include "log.hpp"
#include "slash_pch.hpp"

namespace slash {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application() {
  SL_CORE_ASSERT(!p_application_, "Application already exists");
  p_application_ = this;

  window_ = std::shared_ptr<Window>(Window::Create());
  window_->SetEventCallback(BIND_EVENT_FN(OnEvent));

  Renderer::Create<VulkanRendererAPI>();
  Renderer::AddWindow(window_);
  Renderer::Init();

  ResourceManager::Init();
}

void Application::PushLayer(Layer *layer) { layer_stack_.PushLayer(layer); }

void Application::PushOverlay(Layer *overlay) {
  layer_stack_.PushOverlay(overlay);
}

void Application::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);
  if (dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose))) {
    return;
  }
  dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

  for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
    (*--it)->OnEvent(e);
    if (e.Handled)
      break;
  }
}

void Application::run() {
  while (running_) {
    float time = glfwGetTime();
    TimeStep timestep(time - last_frame_time_);
    last_frame_time_ = time;

    if (!minimized_) {
      for (Layer *layer : layer_stack_)
        layer->OnUpdate(timestep);
    }
    window_->OnUpdate();
    if (!minimized_)
      Renderer::DrawFrame(timestep.GetSecond());
  }
  ResourceManager::Destroy();
  Renderer::Destroy();
}

bool Application::OnWindowClose(WindowCloseEvent &e) {
  e.Handled = true;
  running_ = false;
  return true;
}

bool Application::OnWindowResize(WindowResizeEvent &e) {
  if (e.GetWidth() == 0 || e.GetHeigth() == 0) {
    minimized_ = true;
    return false;
  }

  //Renderer::OnWindowResize(e.GetWidth(), e.GetHeigth());

  minimized_ = false;

  return false;
}
} // namespace slash