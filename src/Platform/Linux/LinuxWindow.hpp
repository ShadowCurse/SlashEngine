#ifndef SL_LINUXWINDOW
#define SL_LINUXWINDOW

#include "Core/Window.hpp"
#include <GLFW/glfw3.h>

namespace Slash
{

    class LinuxWindow: public Window
    {
    public:
        LinuxWindow(const WindowProps& props);
        virtual ~LinuxWindow();

        void OnUpdate() final;

        inline uint GetWidth() const final { return _data.Width; };
        inline uint GetHeight() const final { return _data.Height; };

        inline void SetEventCallback(const EventCallBackFn& callback) final
        {
            _data.EventCallback = callback;
        }
        void SetVSync(bool enabled) final;
        bool IsVSync() const final;

        inline void* GetNativeWindow() const final { return _window; }
    private:
        void Init(const WindowProps& props);
        void Shutdown();

        GLFWwindow* _window;
        VkInstance instance;
        
        struct WindowData
        {
            std::string Title;
            uint Width, Height;
            bool VSync;

            EventCallBackFn EventCallback;
        };

        WindowData _data;
    };

} // namespace Slash

#endif