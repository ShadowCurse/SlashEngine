#include "slash_pch.hpp"
#include "linux_input.hpp"
#include "Core/app.hpp"
#include "GLFW/glfw3.h"

namespace slash {

bool LinuxInput::IsKeyPressedImpl(int keycode) {
  auto window = static_cast<GLFWwindow *>(
      App::Get().GetWindow().GetNativeWindow());
  auto state = glfwGetKey(window, keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool LinuxInput::IsMouseButtonPressedImpl(int button) {
  auto window = static_cast<GLFWwindow *>(
      App::Get().GetWindow().GetNativeWindow());
  auto state = glfwGetMouseButton(window, button);
  return state == GLFW_PRESS;
}

std::pair<float, float> LinuxInput::GetMousePositionImpl() {
  auto window = static_cast<GLFWwindow *>(
      App::Get().GetWindow().GetNativeWindow());
  double x_pos, y_pos;
  glfwGetCursorPos(window, &x_pos, &y_pos);
  return {x_pos, y_pos};
}

float LinuxInput::GetMouseXImpl() {
  auto[x, y] = GetMousePositionImpl();
  return x;
}

float LinuxInput::GetMouseYImpl() {
  auto[x, y] = GetMousePositionImpl();
  return y;
}
} // namespace Slash
