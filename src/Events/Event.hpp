#ifndef SL_EVENT
#define SL_EVENT

#include "Core/Core.hpp"

namespace Slash
{
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled, 
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput       = 1 << 1,
        EventCategoryKeyboard    = 1 << 2,
        EventCategoryMouse       = 1 << 3,
        EventCategoryMouseButton = 1 << 4,
        EventCategoryMouseScroll = 1 << 5,
    };

    class Slash_API Event
    {
    public:
        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0; // only for debug
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }
        inline bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
    };


    #define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                                    virtual EventType GetEventType() const override { return GetStaticType(); }\
                                    virtual const char* GetName() const override { return #type; }

    #define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
    public:
        EventDispatcher(Event& event)
            : _event(event)
        {
        }

        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            if (_event.GetEventType() == T::GetStaticType())
            {
                _event.Handled == func(static_cast<T&>(_event));
                return true;
            }            
            return false;
        }
    private:
        Event& _event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        os << e.ToString();
    }
} // namespace Slash


#endif