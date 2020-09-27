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
  virtual ~Application() = default;

  void run();
  void OnEvent(Event &e);
  void PushLayer(Layer *layer);
  void PushOverlay(Layer *layer);

  auto GetWindow() -> Window & { return *window_; }
  static auto Get() ->  Application & { return *application_; }

 private:
  bool OnWindowClose(WindowCloseEvent &e);
  bool OnWindowResize(WindowResizeEvent &e);

  Shared<Window> window_;
  LayerStack layer_stack_;
  bool running_ = true;
  bool minimized_ = false;
  double last_frame_time_ = 0.0;

  inline static Application *application_ = nullptr;
};

Application *CreateApplication();

} // namespace slash

#endif // SLASHENGINE_CORE_APPLICATION_H_