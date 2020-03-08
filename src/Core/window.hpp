#ifndef SL_WINDOW
#define SL_WINDOW

#include "Events/event.hpp"
#include "core.hpp"
#include "slash_pch.hpp"

namespace slash {

struct WindowProps {
  explicit WindowProps(std::string title = "SlashEngine", uint width = 1920,
                       uint height = 1080)
      : title(std::move(title)), width(width), height(height) {}

  std::string title;
  uint width;
  uint height;
};

class Slash_API Window {
public:
  using EventCallBackFn = std::function<void(Event &)>;
  struct WindowData {
    std::string title;
    uint width, height;
    bool VSync;
    bool resized = false;

    EventCallBackFn EventCallback;
  };

  virtual ~Window() = default;

  virtual void OnUpdate() = 0;
  [[nodiscard]] virtual uint GetWidth() const = 0;
  [[nodiscard]] virtual uint GetHeight() const = 0;
  virtual void SetVSync(bool enabled) = 0;
  [[nodiscard]] virtual bool IsVSync() const = 0;

  virtual void SetEventCallback(const EventCallBackFn &callback) = 0;
  [[nodiscard]] virtual void *GetNativeWindow() const = 0;

  static Window *Create(const WindowProps &props = WindowProps());
};

} // namespace slash

#endif