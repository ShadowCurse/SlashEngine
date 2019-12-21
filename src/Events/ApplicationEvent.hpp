#ifndef SL_APPLICATIONEVENT
#define SL_APPLICATIONEVENT

#include "Event.hpp"

namespace Slash
{
    class Slash_API WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
    class Slash_API WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint width, uint height)
            : _width(width), _height(height) {}

        inline uint GetWidth() const { return _width; }
        inline uint GetHeigth() const { return _height; }

        std::string ToString() const override
        {
            std::stringstream ss;
			ss << "WindowResize: " << _width << ", " << _height;
			return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        uint _width;
        uint _height;
    };
    class Slash_API WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent() {}

        EVENT_CLASS_TYPE(WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)        
    };
    class Slash_API WindowMovedEvent : public Event
    {
    public:
        WindowMovedEvent() {}

        EVENT_CLASS_TYPE(WindowMoved)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)   
    };
    class Slash_API AppTickEvent : public Event
    {    
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)   
        

    };
    class Slash_API AppUpdateEvent : public Event
    {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)   

    };
    class Slash_API AppRenderEvent : public Event
    {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)   

    };
} // namespace Slash

#endif