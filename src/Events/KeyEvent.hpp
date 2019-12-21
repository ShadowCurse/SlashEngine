#ifndef SL_KEYEVENT
#define SL_KEYEVENT

#include "Event.hpp"

namespace Slash
{
    class Slash_API KeyEvent :public Event
    {
    public:
        inline int GetKeyCode() const { return _keyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(int keyCode)
            : _keyCode(keyCode) {}

        int _keyCode;
    };

    class Slash_API KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(int keycoed, int repeatCount)
            : KeyEvent(keycoed), _repeatCount(repeatCount) {}

        inline int GetRepeatCount() const { return _repeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
			ss << "KeyPressedEvent: " << _keyCode << ", " << _repeatCount;
			return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        int _repeatCount;
    };

    class Slash_API KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keyCode)
            : KeyEvent(keyCode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
			ss << "KeyReleasedEvent: " << _keyCode;
			return ss.str();
        }
        EVENT_CLASS_TYPE(KeyReleased)
    };

    class Slash_API KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << _keyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
} // namespace Slash

#endif