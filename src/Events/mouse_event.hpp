#ifndef SLASHENGINE_EVENT_MOUSE_EVENT_H_
#define SLASHENGINE_EVENT_MOUSE_EVENT_H_

//#include "key_event.hpp"
//
//namespace slash {
//
//class Slash_API MouseMovedEvent {
// public:
//  MouseMovedEvent(double x, double y) : x_{x}, y_{y} {}
//
//  [[nodiscard]] auto x() const -> double { return x_; }
//  [[nodiscard]] auto y() const -> double { return y_; }
//
////  [[nodiscard]] auto ToString() const -> std::string override {
////    std::stringstream ss;
////    ss << "MouseMovedEvent: " << mouse_x_ << ", " << mouse_y_;
////    return ss.str();
////  }
//
// private:
//  double x_{};
//  double y_{};
//};
////
////class Slash_API MouseScrolledEvent : public Event {
//// public:
////  MouseScrolledEvent(double xOffset, double yOffset)
////      : x_offset_(xOffset), y_offset_(yOffset) {}
////
////  [[nodiscard]] auto GetXOffset() const -> double { return x_offset_; }
////  [[nodiscard]] auto GetYOffset() const -> double { return y_offset_; }
////
////  [[nodiscard]] auto ToString() const -> std::string override {
////    std::stringstream ss;
////    ss << "MouseMovedEvent: " << GetXOffset() << ", " << GetYOffset();
////    return ss.str();
////  }
////
////  EVENT_CLASS_TYPE(MouseScrolled)
////  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
////
//// private:
////  double x_offset_;
////  double y_offset_;
////};
////
////class Slash_API MouseButtonEvent : public Event {
//// public:
////  [[nodiscard]] auto GetMouseButton() const -> int { return button_; }
////
////  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
////
//// protected:
////  explicit MouseButtonEvent(int button) : button_(button) {}
////  int button_;
////};
////
//class Slash_API MouseButtonPressedEvent {
// public:
//  explicit MouseButtonPressedEvent(int button) : button_{button} {}
//
//  [[nodiscard]] auto button() const -> int { return button_; }
//
////  [[nodiscard]] auto ToString() const -> std::string override {
////    std::stringstream ss;
////    ss << "MouseButtonPressedEvent: " << button_;
////    return ss.str();
////  }
// private:
//  int button_{};
//};
//
//class Slash_API MouseButtonReleasedEvent {
// public:
//  explicit MouseButtonReleasedEvent(int button) : button_{button} {}
//
//  [[nodiscard]] auto button() const -> int { return button_; }
//
////  [[nodiscard]] auto ToString() const -> std::string override {
////    std::stringstream ss;
////    ss << "MouseButtonPressedEvent: " << button_;
////    return ss.str();
////  }
// private:
//  int button_{};
//};
//
//} // namespace slash

#endif // SLASHENGINE_EVENT_MOUSE_EVENT_H_