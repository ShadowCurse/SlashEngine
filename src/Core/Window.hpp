#ifndef SL_WINDOW
#define SL_WINDOW

#include "SlashPCH.hpp"

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
        uint Height;
        uint Width;
    };

    class Slash_API Window
    {
    public:
        using EventCallBackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;
        
        virtual uint GetWidth() const = 0;
        virtual uint GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallBackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static Window* Create(const WindowProps& props = WindowProps());
    };
    


} // namespace Slash


#endif