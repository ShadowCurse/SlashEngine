#ifndef SLASHENGINE_EVENT_KEY_EVENT_H_
#define SLASHENGINE_EVENT_KEY_EVENT_H_

#include "event.hpp"

namespace slash {

class Slash_API KeyEvent : public Event {
 public:
  [[nodiscard]] auto GetKeyCode() const -> int { return key_code_; }

  EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
 protected:
  explicit KeyEvent(int keyCode) : key_code_(keyCode) {}

  int key_code_;
};

class Slash_API KeyPressedEvent : public KeyEvent {
 public:
  KeyPressedEvent(int keycode, int repeatCount)
      : KeyEvent(keycode), repeat_count_(repeatCount) {}

  [[nodiscard]] auto GetRepeatCount() const -> int { return repeat_count_; }

  [[nodiscard]] auto ToString() const -> std::string final {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << key_code_ << ", " << repeat_count_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(KeyPressed)
 private:
  int repeat_count_;
};

class Slash_API KeyReleasedEvent : public KeyEvent {
 public:
  explicit KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

  [[nodiscard]] auto ToString() const -> std::string final {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << key_code_;
    return ss.str();
  }
  EVENT_CLASS_TYPE(KeyReleased)
};

class Slash_API KeyTypedEvent : public KeyEvent {
 public:
  explicit KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

  [[nodiscard]] auto ToString() const -> std::string final {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << key_code_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(KeyTyped)
};

} // namespace slash

#endif // SLASHENGINE_EVENT_KEY_EVENT_H_