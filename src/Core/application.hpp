#ifndef SLASHENGINE_CORE_APPLICATION_H_
#define SLASHENGINE_CORE_APPLICATION_H_

#include "Events/application_event.hpp"
#include "Events/event.hpp"
#include "core.hpp"
#include "layer_stack.hpp"
#include "time_step.hpp"
#include "window.hpp"

namespace slash {

class Slash_API Application {
public:
  Application();
  virtual ~Application();

  void run();
  void OnEvent(Event &e);
  void PushLayer(Layer *layer);
  void PushOverlay(Layer *layer);

  inline Window &GetWindow() { return *window_; }
  inline static Application &Get() { return *p_application_; }

private:
  bool OnWindowClose(WindowCloseEvent &e);
  bool OnWindowResize(WindowResizeEvent &e);

  Shared<Window> window_;
  bool running_ = true;
  bool minimized_ = false;
  LayerStack layer_stack_;
  float last_frame_time_ = 0.0f;

  inline static Application *p_application_ = nullptr;
};

Application *CreateApplication();
} // namespace slash
#endif // SLASHENGINE_CORE_APPLICATION_H_