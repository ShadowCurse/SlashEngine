#include "application.hpp"
#include "log.hpp"
#include "slash_pch.hpp"
//#include "GLFW/glfw3.h"
#include "Renderer/renderer.hpp"
#include "VulkanRenderer/VulkanRendererAPI.hpp"

namespace slash {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

// Application *Application::p_application_ = nullptr;

Application::Application() {
  SL_CORE_ASSERT(!p_application_, "Application already exists");
  p_application_ = this;

  window_ = std::shared_ptr<Window>(Window::Create());
  window_->SetEventCallback(BIND_EVENT_FN(OnEvent));

  Renderer::Create<VulkanRendererAPI>();
  Renderer::AddWindow(window_);
  // auto conf = Renderer::GetAPI<VulkanRendererAPI>().GetConfig();
  Renderer::Init();
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

Application::~Application() {}

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

  minimized_ = false;

  return false;
}
} // namespace slash