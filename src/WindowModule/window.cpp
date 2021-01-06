#include "window.hpp"

#include "Core/log.hpp"
#include "Events/application_event.hpp"
#include "Events/key_event.hpp"
#include "Events/mouse_event.hpp"

namespace slash {

Window::Window(WindowData data) {
  data_ = std::move(data);

  SL_CORE_INFO("Creating window Title: {0}, Width: {1}, Height: {2}",
               data_.title, data_.width, data_.height);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(static_cast<int>(data_.width),
                             static_cast<int>(data_.height),
                             data_.title.c_str(), nullptr, nullptr);

  glfwSetWindowUserPointer(window_, &data_);

  glfwSetWindowCloseCallback(window_, [](GLFWwindow *window) {
    auto data =
        static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->close_callback(data->id);
  });

//  set_event_callback([&](Event& event) {
//    SL_TRACE("Window : {0}, event: {1}", data_.id, event.ToString());
//  });
}

Window::~Window() {
  glfwDestroyWindow(window_);
}

auto Window::id() const -> size_t { return data_.id; }
auto Window::width() const -> uint { return data_.width; }
auto Window::height() const -> uint { return data_.height; }
auto Window::vsync() const -> bool { return data_.VSync; }

//void Window::SetVSync([[maybe_unused]] bool enabled) {
// if(enabled)
//     glfwSwapInterval(1);
// else
//     glfwSwapInterval(0);
// _data.VSync = enabled;
//}

void Window::set_event_callback(const event_callback_fn &callback) {
  data_.event_callback = callback;
  glfwSetWindowSizeCallback(
      window_, [](GLFWwindow *window, int width, int height) {
        auto data =
            static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data->width = static_cast<uint>(width);
        data->height = static_cast<uint>(height);

        WindowResizeEvent event(data->width, data->height);
        data->event_callback(event);
      });

  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width,
                                             int heigh) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->resized = true;

    WindowResizeEvent event(static_cast<uint>(width), static_cast<uint>(heigh));
    data->event_callback(event);
  });

  glfwSetKeyCallback(window_, [](GLFWwindow *window, int key,
                                 [[maybe_unused]] int scancode, int action,
                                 [[maybe_unused]] int mods) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        KeyPressedEvent event(key, 0);
        data->event_callback(event);
        break;
      }
      case GLFW_RELEASE: {
        KeyReleasedEvent event(key);
        data->event_callback(event);
        break;
      }
      case GLFW_REPEAT: {
        KeyPressedEvent event(key, 1);
        data->event_callback(event);
        break;
      }
      default:break;
    }
  });

  glfwSetCharCallback(window_, [](GLFWwindow *window, uint keycode) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    KeyTypedEvent event(static_cast<int>(keycode));
    data->event_callback(event);
  });

  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button,
                                         int action,
                                         [[maybe_unused]] int mods) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        MouseButtonPressedEvent event(button);
        data->event_callback(event);
        break;
      }
      case GLFW_RELEASE: {
        MouseButtonReleasedEvent event(button);
        data->event_callback(event);
        break;
      }
      default:break;
    }
  });

  glfwSetScrollCallback(
      window_, [](GLFWwindow *window, double xOffset, double yOffset) {
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseScrolledEvent event(xOffset, yOffset);
        data->event_callback(event);
      });

  glfwSetCursorPosCallback(
      window_, [](GLFWwindow *window, double xPos, double yPos) {
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseMovedEvent event(xPos, yPos);
        data->event_callback(event);
      });
}

GLFWwindow *Window::get_native_window() const { return window_; }

} // namespace slash