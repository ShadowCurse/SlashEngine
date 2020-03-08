#ifndef SLASHENGINE_EVENT_MOUSE_EVENT_H_
#define SLASHENGINE_EVENT_MOUSE_EVENT_H_

#include "key_event.hpp"

namespace slash {

class Slash_API MouseMovedEvent : public Event {
public:
  MouseMovedEvent(float x, float y) : mouse_x_(x), mouse_y_(y) {}

  [[nodiscard]] float GetX() const { return mouse_x_; }
  [[nodiscard]] inline float GetY() const { return mouse_y_; }

  [[nodiscard]] std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << mouse_x_ << ", " << mouse_y_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseMoved)
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
  float mouse_x_;
  float mouse_y_;
};

class Slash_API MouseScrolledEvent : public Event {
public:
  MouseScrolledEvent(float xOffset, float yOffset)
      : x_offset_(xOffset), y_offset_(yOffset) {}

  [[nodiscard]] inline float GetXOffset() const { return x_offset_; }
  [[nodiscard]] inline float GetYOffset() const { return y_offset_; }

  [[nodiscard]] std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << GetXOffset() << ", " << GetYOffset();
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseScrolled)
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
  float x_offset_;
  float y_offset_;
};

class Slash_API MouseButtonEvent : public Event {
public:
  [[nodiscard]] inline int GetMouseButton() const { return button_; }

  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

protected:
  explicit MouseButtonEvent(int button) : button_(button) {}
  int button_;
};

class Slash_API MouseButtonPressedEvent : public MouseButtonEvent {
public:
  explicit MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

  [[nodiscard]] std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << button_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseButtonPressed)
};

class Slash_API MouseButtonReleasedEvent : public MouseButtonEvent {
public:
  explicit MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

  [[nodiscard]] std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << button_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseButtonReleased)
};

} // namespace slash

#endif // SLASHENGINE_EVENT_MOUSE_EVENT_H_