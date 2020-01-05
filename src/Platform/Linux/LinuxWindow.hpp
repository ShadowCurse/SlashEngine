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
        uint GetWidth() const final;
        uint GetHeight() const final;
        bool IsVSync() const final;
        void SetVSync(bool enabled) final;
        void SetEventCallback(const EventCallBackFn& callback) final;
        void* GetNativeWindow() const final;
    private:
        void Init(const WindowProps& props);
        void Shutdown();

        GLFWwindow* _window;
        WindowData _data;
    };

} // namespace Slash

#endif