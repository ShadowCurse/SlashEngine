#include "LinuxWindow.hpp"

#include "Events/ApplicationEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Events/KeyEvent.hpp"

#include "Core/Log.hpp"

namespace Slash
{

    static bool _GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        SL_CORE_ERROR("GLFW error ({0}): {1}", error, description);
    }

    // body of static Create method in core/Window.hpp 
    Window* Window::Create(const WindowProps& props)
    {
        return new LinuxWindow(props);
    }

    LinuxWindow::LinuxWindow(const WindowProps& props)
    {
        Init(props);
    }

    LinuxWindow::~LinuxWindow()
    {
        Shutdown();
    }

    void LinuxWindow::Init(const WindowProps& props)
    {
        _data.Title  = props.Title;
        _data.Width  = props.Width;
        _data.Height = props.Height;

        SL_CORE_INFO("Creating window Title: {0}, Width: {1}, Height: {2}", props.Title, props.Width, props.Height);

        if (!_GLFWInitialized)
        {
            int success = glfwInit();
            SL_CORE_ASSERT(success, "Could not initialized GLFW with Vulcan");
            glfwSetErrorCallback(GLFWErrorCallback);
            _GLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        _window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(_window, &_data);

        glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int heigh)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->Width = width;
            data->Height = heigh;

            WindowResizeEvent event(width, heigh);
            data->EventCallback(event);
        });

        glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            data->EventCallback(event);
        });

        glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    data->EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data->EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    data->EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(_window, [](GLFWwindow* window, uint keycode)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            KeyTypedEvent event(keycode);
            data->EventCallback(event);
        });

        glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            switch(action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    data->EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    data->EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            MouseScrolledEvent event(xOffset, yOffset);
            data->EventCallback(event);
        });

        glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            MouseMovedEvent event(xPos, yPos);
            data->EventCallback(event);
        });
    }

    void LinuxWindow::Shutdown()
    {
        glfwDestroyWindow(_window);

        glfwTerminate();
    }

    void LinuxWindow::OnUpdate()
    {
        glfwPollEvents();
    }

    void LinuxWindow::SetVSync(bool enabled)
    {
        // if(enabled)
        //     glfwSwapInterval(1);
        // else
        //     glfwSwapInterval(0);
        // _data.VSync = enabled;
    }

    bool LinuxWindow::IsVSync() const
    {
        return _data.VSync;
    }

} // namespace Slash