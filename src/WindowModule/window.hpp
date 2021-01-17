#ifndef SLASHENGINE_WINDOWMODULE_WINDOW_H_
#define SLASHENGINE_WINDOWMODULE_WINDOW_H_

#include "Core/core.hpp"
#include "Core/log.hpp"
#include "Core/app.hpp"
#include "Core/event.hpp"
#include <GLFW/glfw3.h>

namespace slash {

class Slash_API Window {
 public:
  using window_close_callback_fn = std::function<void(size_t)>;//void(*)(GLFWwindow*);

  struct WindowData {
    size_t id;
    std::string title;
    uint width;
    uint height;
    bool VSync = false;
    bool resized = false;

    App *app_ptr;
    window_close_callback_fn close_callback;
  };
 public:

  explicit Window(WindowData data);
  ~Window();

  [[nodiscard]] auto id() const -> size_t;
  [[nodiscard]] auto width() const -> uint;
  [[nodiscard]] auto height() const -> uint;
  [[nodiscard]] auto vsync() const -> bool;

//  void set_event_callback(const event_callback_fn &callback);
//  void SetVSync(bool enabled);

  [[nodiscard]] auto is_key_pressed(int keycode) const -> bool;
  [[nodiscard]] auto is_mouse_button_pressed(int button) const -> bool;
  [[nodiscard]] auto mouse_pos() const -> std::pair<int, int>;

  [[nodiscard]] auto get_native_window() const -> GLFWwindow *;

 private:
  GLFWwindow *window_ = nullptr;
  WindowData data_;
};

} // namespace slash

#endif // SLASHENGINE_WINDOWMODULE_WINDOW_H_