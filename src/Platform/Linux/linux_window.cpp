#include "linux_window.hpp"

#include "Core/log.hpp"
#include "Events/application_event.hpp"
#include "Events/key_event.hpp"
#include "Events/mouse_event.hpp"

namespace slash {

static void GLFWErrorCallback(int error, const char *description) {
  SL_CORE_ERROR("GLFW error ({0}): {1}", error, description);
}

// body of static Create method in core/Window.hpp
Window *Window::Create(const WindowProps &props) {
  return new LinuxWindow(props);
}

LinuxWindow::LinuxWindow(const WindowProps &props) { Init(props); }

LinuxWindow::~LinuxWindow() { Shutdown(); }

void LinuxWindow::OnUpdate() { glfwPollEvents(); }

uint LinuxWindow::GetWidth() const { return data_.width; }
uint LinuxWindow::GetHeight() const { return data_.height; }

bool LinuxWindow::IsVSync() const { return data_.VSync; }

void LinuxWindow::SetVSync([[maybe_unused]] bool enabled) {
  // if(enabled)
  //     glfwSwapInterval(1);
  // else
  //     glfwSwapInterval(0);
  // _data.VSync = enabled;
}

void LinuxWindow::SetEventCallback(const EventCallBackFn &callback) {
  data_.EventCallback = callback;
}

void *LinuxWindow::GetNativeWindow() const { return window_; }

void LinuxWindow::Init(const WindowProps &props) {
  data_.title = props.title;
  data_.width = props.width;
  data_.height = props.height;

  SL_CORE_INFO("Creating window Title: {0}, Width: {1}, Height: {2}",
               props.title, props.width, props.height);

  if (!GLFW_initialized_) {
    int success = glfwInit();
    SL_CORE_ASSERT(success, "Could not initialized GLFW with Vulkan");
    glfwSetErrorCallback(GLFWErrorCallback);
    GLFW_initialized_ = true;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(static_cast<int>(props.width),
                             static_cast<int>(props.height),
                             props.title.c_str(), nullptr, nullptr);

  glfwSetWindowUserPointer(window_, &data_);

  glfwSetWindowSizeCallback(
      window_, [](GLFWwindow *window, int width, int height) {
        WindowData *data =
            static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data->width = static_cast<uint>(width);
        data->height = static_cast<uint>(height);

        WindowResizeEvent event(data->width, data->height);
        data->EventCallback(event);
      });

  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width,
                                             int heigh) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->resized = true;

    WindowResizeEvent event(static_cast<uint>(width), static_cast<uint>(heigh));
    data->EventCallback(event);
  });

  glfwSetWindowCloseCallback(window_, [](GLFWwindow *window) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    WindowCloseEvent event;
    data->EventCallback(event);
  });

  glfwSetKeyCallback(window_, [](GLFWwindow *window, int key,
                                 [[maybe_unused]] int scancode, int action,
                                 [[maybe_unused]] int mods) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        KeyPressedEvent event(key, 0);
        data->EventCallback(event);
        break;
      }
      case GLFW_RELEASE: {
        KeyReleasedEvent event(key);
        data->EventCallback(event);
        break;
      }
      case GLFW_REPEAT: {
        KeyPressedEvent event(key, 1);
        data->EventCallback(event);
        break;
      }
      default:break;
    }
  });

  glfwSetCharCallback(window_, [](GLFWwindow *window, uint keycode) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    KeyTypedEvent event(static_cast<int>(keycode));
    data->EventCallback(event);
  });

  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button,
                                         int action,
                                         [[maybe_unused]] int mods) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        MouseButtonPressedEvent event(button);
        data->EventCallback(event);
        break;
      }
      case GLFW_RELEASE: {
        MouseButtonReleasedEvent event(button);
        data->EventCallback(event);
        break;
      }
      default:break;
    }
  });

  glfwSetScrollCallback(
      window_, [](GLFWwindow *window, double xOffset, double yOffset) {
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseScrolledEvent event(xOffset, yOffset);
        data->EventCallback(event);
      });

  glfwSetCursorPosCallback(
      window_, [](GLFWwindow *window, double xPos, double yPos) {
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseMovedEvent event(xPos, yPos);
        data->EventCallback(event);
      });
}

void LinuxWindow::Shutdown() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

} // namespace slash