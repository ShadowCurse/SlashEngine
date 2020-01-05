#ifndef SL_APPLICATION
#define SL_APPLICATION

#include "Core.hpp"

#include "Window.hpp"
#include "Core/LayerStack.hpp"
#include "Events/Event.hpp"
#include "Core/Timestep.hpp"
#include "Events/ApplicationEvent.hpp"

namespace Slash
{
    class Slash_API Application
    {
    public:
        Application();
        virtual ~Application();

        void run();
        void OnEvent(Event& e);
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline Window& GetWindow() { return *m_window; }
        inline static Application& Get() { return *m_instance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        Shared<Window> m_window;
        bool           m_running = true;
        bool           m_minimized = false;
        LayerStack     m_layerStack;
        float          m_lastFrameTime = 0.0f;

        static Application* m_instance;
    };

    Application* CreateApplication();
} // namespace Slash
#endif