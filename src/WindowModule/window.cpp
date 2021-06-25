#include "window.hpp"

#include "Core/log.hpp"
#include "events.hpp"

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

//  glfwSetWindowSizeCallback(
//      window_, [](GLFWwindow *window, int width, int height) {
//        auto data =
//            static_cast<WindowData *>(glfwGetWindowUserPointer(window));
//        data->width = static_cast<uint>(width);
//        data->height = static_cast<uint>(height);
//
//        WindowResizeEvent event(data->width, data->height);
//        data->event_callback(event);
//      });

//  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width,
//                                             int heigh) {
//    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
//    data->resized = true;
//
//    WindowResizeEvent event(static_cast<uint>(width), static_cast<uint>(heigh));
//    data->event_callback(event);
//  });

  glfwSetKeyCallback(window_, [](GLFWwindow *window, int key,
                                 [[maybe_unused]] int scancode, int action,
                                 [[maybe_unused]] int mods) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        data->event_pool_module->get_event<KeyPressedEvent>().emit(key, 0);
        break;
      }
      case GLFW_RELEASE: {
        data->event_pool_module->get_event<KeyReleasedEvent>().emit(key);
        break;
      }
      case GLFW_REPEAT: {
        data->event_pool_module->get_event<KeyPressedEvent>().emit(key, 1);
        break;
      }
      default:break;
    }
  });

  glfwSetCharCallback(window_, [](GLFWwindow *window, uint keycode) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->event_pool_module->get_event<KeyTypedEvent>().emit(static_cast<int>(keycode));
  });

  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button,
                                         int action,
                                         [[maybe_unused]] int mods) {
    auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        data->event_pool_module->get_event<MouseButtonPressedEvent>().emit(button);
        break;
      }
      case GLFW_RELEASE: {
        data->event_pool_module->get_event<MouseButtonReleasedEvent>().emit(button);
        break;
      }
      default:break;
    }
  });

  glfwSetScrollCallback(
      window_, [](GLFWwindow *window, double x_offset, double y_offset) {
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data->event_pool_module->get_event<MouseScrolledEvent>().emit(x_offset, y_offset);
      });

  glfwSetCursorPosCallback(
      window_, [](GLFWwindow *window, double xPos, double yPos) {
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data->event_pool_module->get_event<MouseMovedEvent>().emit(xPos, yPos);
      });
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

auto Window::is_key_pressed(int keycode) const -> bool {
  auto state = glfwGetKey(window_, keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}
auto Window::is_mouse_button_pressed(int button) const -> bool {
  auto state = glfwGetMouseButton(window_, button);
  return state == GLFW_PRESS;
}
auto Window::mouse_pos() const -> std::pair<int, int> {
  double x_pos, y_pos;
  glfwGetCursorPos(window_, &x_pos, &y_pos);
  return {x_pos, y_pos};
}

auto Window::get_native_window() const -> GLFWwindow * { return window_; }

} // namespace slash