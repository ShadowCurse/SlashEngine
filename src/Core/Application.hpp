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

        inline Window& GetWindow() { return *_window; }

        inline static Application& Get() { return *_instance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        Unique<Window> _window;
        bool _running = true;
        bool _minimized = false;
        LayerStack _layerStack;
        float _lastFrameTime = 0.0f;

        static Application* _instance;
    };

    Application* CreateApplication();
} // namespace Slash
#endif