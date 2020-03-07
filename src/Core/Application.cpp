#include "Application.hpp"
#include "Core/Log.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/Renderer.hpp"
#include "VulkanRenderer/VulkanRendererAPI.hpp"
#include "slash_pch.hpp"

namespace Slash
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::m_instance = nullptr;

    Application::Application()
    {
        SL_CORE_ASSERT(!m_instance, "Application already exists");
        m_instance = this;

        m_window = std::shared_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        Renderer::Create<VulkanRendererAPI>();
        Renderer::AddWindow(m_window);
        // auto conf = Renderer::GetAPI<VulkanRendererAPI>().GetConfig();
        Renderer::Init();
    }

    void Application::PushLayer(Layer* layer)
    {
        m_layerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_layerStack.PushOverlay(overlay);
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        if (dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose))) {
            return;
        }
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
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
        while (m_running)
        {
            float time = glfwGetTime();
            Timestep timestep = time - m_lastFrameTime;
            m_lastFrameTime = time;

            if (!m_minimized)
            {
                for (Layer* layer: m_layerStack)
                    layer->OnUpdate(timestep);
            }
            m_window->OnUpdate();
            if (!m_minimized)
                Renderer::DrawFrame(timestep.GetSecond());
        }
        Renderer::Destroy();
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        e.Handled = true;
        m_running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeigth() == 0)
        {
            m_minimized = true;
            return false;
        }

        m_minimized = false;

        return false;
    }
} // namespace Slash