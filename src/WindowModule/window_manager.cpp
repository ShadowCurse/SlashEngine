#include "window_manager.hpp"

#include "Core/log.hpp"

namespace slash {

WindowManager::WindowManager(App& app) : app_ptr_{&app}{
  SL_CORE_ASSERT(glfwInit(), "Could not initialized GLFW")
  glfwSetErrorCallback(error_callback);
  app.add_system([]{ update(); });
}

WindowManager::~WindowManager() {
  glfwTerminate();
}

void WindowManager::add_window(WindowParams params) {

  Window::WindowData data{
      .id = windows_.size(),
      .title = std::move(params.title),
      .width = params.width,
      .height = params.height,
      .app_ptr = app_ptr_,
      .close_callback = [&](size_t window_id) { window_close_callback(window_id); }
  };

  windows_.emplace_back(std::make_unique<Window>(std::move(data)));
}

auto WindowManager::get_windows() const -> const std::vector<std::unique_ptr<Window>> & {
  return windows_;
}

void WindowManager::update() {
  glfwPollEvents();
}

void WindowManager::error_callback(int error, const char *description) {
  SL_CORE_ERROR("GLFW error ({0}): {1}", error, description);
}

void WindowManager::window_close_callback(size_t window_id) {
  std::erase_if(windows_, [&](const auto &window) { return window->id() == window_id; });
  if (windows_.empty())
    app_ptr_->get_event<AppClose>().emit();
}

}