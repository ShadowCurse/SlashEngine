#ifndef SL_WINDOW
#define SL_WINDOW

#include "slash_pch.hpp"

#include "Core/Core.hpp"
#include "Events/Event.hpp"

namespace Slash
{
    
    struct WindowProps
    {
        WindowProps(const std::string& title = "SlashEngine",
                    uint width = 1920,
                    uint height = 1080)
                    : Title(title),
                      Width(width),
                      Height(height)
        {} 

        std::string Title;
        uint Width;
        uint Height;
    };

    class Slash_API Window
    {
    public:
        using EventCallBackFn = std::function<void(Event&)>;
        struct WindowData
        {
            std::string Title;
            uint Width, Height;
            bool VSync;
            bool resized = false;

            EventCallBackFn EventCallback;
        };
    public:

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;
        virtual uint GetWidth() const = 0;
        virtual uint GetHeight() const = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void SetEventCallback(const EventCallBackFn& callback) = 0;
        virtual void* GetNativeWindow() const = 0;

        static Window* Create(const WindowProps& props = WindowProps());
    };
    


} // namespace Slash


#endif