#include "Application.hpp"

#include "SlashPCH.hpp"
#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "GLFW/glfw3.h"

namespace Slash
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::_instance = nullptr;

    Application::Application()
    {
        SL_CORE_ASSERT(!_instance, "Application already exists");
        _instance = this;

        _window = std::unique_ptr<Window>(Window::Create());
        _window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        Renderer::Init();
    }

    void Application::PushLayer(Layer* layer)
    {
        _layerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        _layerStack.PushOverlay(overlay);
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        if (dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose))) {
            return;
        }
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        for (auto it = _layerStack.end(); it != _layerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }

    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        while (_running)
        {
            float time = glfwGetTime();
            Timestep timestep = time - _lastFrameTime;
            _lastFrameTime = time;

            if (!_minimized)
            {
                for (Layer* layer: _layerStack)
                    layer->OnUpdate(timestep);
            }

            _window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        Renderer::Destroy();
        _running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeigth() == 0)
        {
            _minimized = true;
            return false;
        }

        _minimized = false;

        return false;
    }
} // namespace Slash