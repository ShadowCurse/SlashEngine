#ifndef SLASHENGINE_PLATFORM_LINUX_LINUX_WINDOW_H_
#define SLASHENGINE_PLATFORM_LINUX_LINUX_WINDOW_H_

#include "Core/window.hpp"
#include <GLFW/glfw3.h>

namespace slash {

class Slash_API LinuxWindow final : public Window {
 public:
  explicit LinuxWindow(const WindowProps &props);
  ~LinuxWindow() final;

  void OnUpdate() final;
  uint GetWidth() const final;
  uint GetHeight() const final;
  bool IsVSync() const final;
  void SetVSync(bool enabled) final;
  void SetEventCallback(const EventCallBackFn &callback) final;
  void *GetNativeWindow() const final;

 private:
  void Init(const WindowProps &props);
  void Shutdown();

  static inline bool GLFW_initialized_ = false;
  GLFWwindow *window_ = nullptr;
  WindowData data_;
};

} // namespace slash

#endif // SLASHENGINE_PLATFORM_LINUX_LINUX_WINDOW_H_