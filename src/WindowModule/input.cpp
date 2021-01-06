#include "slash_pch.hpp"
#include "input.hpp"
#include "GLFW/glfw3.h"

namespace slash {

//bool Input::IsKeyPressed(int keycode) {
//  auto window = static_cast<GLFWwindow *>(
//      App::Get().GetWindow().GetNativeWindow());
//  auto state = glfwGetKey(window, keycode);
//  return state == GLFW_PRESS || state == GLFW_REPEAT;
//}
//
//bool Input::IsMouseButtonPressedImpl(int button) {
//  auto window = static_cast<GLFWwindow *>(
//      App::Get().GetWindow().GetNativeWindow());
//  auto state = glfwGetMouseButton(window, button);
//  return state == GLFW_PRESS;
//}
//
//std::pair<float, float> Input::GetMousePositionImpl() {
//  auto window = static_cast<GLFWwindow *>(
//      App::Get().GetWindow().GetNativeWindow());
//  double x_pos, y_pos;
//  glfwGetCursorPos(window, &x_pos, &y_pos);
//  return {x_pos, y_pos};
//}
//
//float Input::GetMouseXImpl() {
//  auto[x, y] = GetMousePositionImpl();
//  return x;
//}
//
//float Input::GetMouseYImpl() {
//  auto[x, y] = GetMousePositionImpl();
//  return y;
//}
} // namespace Slash
