#ifndef SLASHENGINE_EVENT_MOUSE_EVENT_H_
#define SLASHENGINE_EVENT_MOUSE_EVENT_H_

#include "key_event.hpp"

namespace slash {

class Slash_API MouseMovedEvent : public Event {
 public:
  MouseMovedEvent(double x, double y) : mouse_x_(x), mouse_y_(y) {}

  [[nodiscard]] auto GetX() const -> double { return mouse_x_; }
  [[nodiscard]] auto GetY() const -> double { return mouse_y_; }

  [[nodiscard]] auto ToString() const -> std::string override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << mouse_x_ << ", " << mouse_y_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseMoved)
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

 private:
  double mouse_x_;
  double mouse_y_;
};

class Slash_API MouseScrolledEvent : public Event {
 public:
  MouseScrolledEvent(double xOffset, double yOffset)
      : x_offset_(xOffset), y_offset_(yOffset) {}

  [[nodiscard]] auto GetXOffset() const -> double { return x_offset_; }
  [[nodiscard]] auto GetYOffset() const -> double { return y_offset_; }

  [[nodiscard]] auto ToString() const -> std::string override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << GetXOffset() << ", " << GetYOffset();
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseScrolled)
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

 private:
  double x_offset_;
  double y_offset_;
};

class Slash_API MouseButtonEvent : public Event {
 public:
  [[nodiscard]] auto GetMouseButton() const -> int { return button_; }

  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

 protected:
  explicit MouseButtonEvent(int button) : button_(button) {}
  int button_;
};

class Slash_API MouseButtonPressedEvent : public MouseButtonEvent {
 public:
  explicit MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

  [[nodiscard]] auto ToString() const -> std::string override {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << button_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseButtonPressed)
};

class Slash_API MouseButtonReleasedEvent : public MouseButtonEvent {
 public:
  explicit MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

  [[nodiscard]] auto ToString() const -> std::string override {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << button_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseButtonReleased)
};

} // namespace slash

#endif // SLASHENGINE_EVENT_MOUSE_EVENT_H_