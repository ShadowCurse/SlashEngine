#include "window_manager.hpp"

#include "Core/log.hpp"

namespace slash {

WindowManager::WindowManager(EventPoolModule &ep, SystemModule &sm) : ep_{ep} {
  if (!glfwInit())
    SL_CORE_ERROR("Could not initialized GLFW");
  glfwSetErrorCallback(error_callback);
  sm.add_system([&] { update(); });
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
      .event_pool_module = &ep_,
      .close_callback = [&](size_t window_id) { window_close_callback(window_id); }
  };

  windows_.emplace_back(std::make_unique<Window>(std::move(data)));
}

auto WindowManager::get_main_window() const -> const Window & {
  return *windows_[0].get();
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
    ep_.get_event<AppClose>().emit();
}

}