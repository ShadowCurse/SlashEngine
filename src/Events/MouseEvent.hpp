#ifndef SL_MOUSEEVENT
#define SL_MOUSEEVENT

#include "KeyEvent.hpp"

namespace Slash
{
    class Slash_API MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : _mouseX(x), _mouseY(y) {}

        inline float GetX() const { return _mouseX; }
        inline float GetY() const { return _mouseY; }

        std::string ToString() const override
        {
            std::stringstream ss;
			ss << "MouseMovedEvent: " << _mouseX << ", " << _mouseY;
			return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float _mouseX;
        float _mouseY;
    };

    class Slash_API MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : _xOffset(xOffset), _yOffset(yOffset) {}
        
        inline float GetXOffset() const { return _xOffset; }
        inline float GetYOffset() const { return _yOffset; }

        std::string ToString() const override
        {
           	std::stringstream ss;
			ss << "MouseMovedEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float _xOffset;
        float _yOffset;
    };

    class Slash_API MouseButtonEvent : public Event
    {
    public:
        inline int GetMouseButton() const { return _button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    protected:
        MouseButtonEvent(int button)
            : _button(button) {}
        int _button;
    };

    class Slash_API MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(int button)
            : MouseButtonEvent(button) {}
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << _button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class Slash_API MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(int button)
            : MouseButtonEvent(button) {}
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << _button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };

    
} // namespace Slash

#endif