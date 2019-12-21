#ifndef SL_LINUXWINDOW
#define SL_LINUXWINDOW

#include "Core/Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Slash
{

    class LinuxWindow: public Window
    {
    public:
        LinuxWindow(const WindowProps& props);
        virtual ~LinuxWindow();

        void OnUpdate() override;

        inline uint GetWidth() const override { return _data.Width; };
        inline uint GetHeight() const override { return _data.Height; };

        inline void SetEventCallback(const EventCallBackFn& callback) override
        {
            _data.EventCallback = callback;
        }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline virtual void* GetNativeWindow() const { return _window; }
    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

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