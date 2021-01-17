#ifndef SL_WINDOW
#define SL_WINDOW

#include "event.hpp"
#include "core.hpp"
#include "slash_pch.hpp"

namespace slash {

//struct WindowParams {
//  explicit WindowParams(std::string title = "SlashEngine", uint width = 1920,
//                       uint height = 1080)
//      : title(std::move(title)), width(width), height(height) {}
//
//  std::string title;
//  uint width;
//  uint height;
//};
//
//class Slash_API Window {
//public:
//  using event_callback_fn = std::function<void(Event &)>;
//  struct WindowData {
//    std::string title;
//    uint width;
//    uint height;
//    bool VSync;
//    bool resized = false;
//
//    event_callback_fn event_callback;
//  };
//
//  virtual ~Window() = default;
//
//  virtual void on_update() = 0;
//  [[nodiscard]] virtual uint get_width() const = 0;
//  [[nodiscard]] virtual uint get_height() const = 0;
//  virtual void SetVSync(bool enabled) = 0;
//  [[nodiscard]] virtual bool is_vsync() const = 0;
//
//  virtual void set_event_callback(const event_callback_fn &callback) = 0;
//  [[nodiscard]] virtual void *GetNativeWindow() const = 0;
//
//  static Window *Create(const WindowParams &props = WindowParams());
//};

} // namespace slash

#endif