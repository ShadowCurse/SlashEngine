#include "Platform/Linux/LinuxInput.hpp"
#include "Core/Application.hpp"
#include "GLFW/glfw3.h"
#include "slash_pch.hpp"

namespace Slash
{

    bool LinuxInput::IsKeyPressedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    
    bool LinuxInput::IsMouseButtonPressedImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> LinuxInput::GetMoucePositionImpl()
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { xpos, ypos };
    }

    float LinuxInput::GetMouseXImpl()
    {
        auto [x, y] = GetMoucePositionImpl();
        return x;
    }

    float LinuxInput::GetMouseYImpl()
    {
        auto [x, y] = GetMoucePositionImpl();
        return y;
    }
} // namespace Slash
