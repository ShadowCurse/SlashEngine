#ifndef SLASHENGINE_EVENTS_APPLICATION_EVENT_H_
#define SLASHENGINE_EVENTS_APPLICATION_EVENT_H_

#include "event.hpp"

namespace slash {

class Slash_API WindowCloseEvent : public Event {
public:
  EVENT_CLASS_TYPE(WindowClose)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class Slash_API WindowResizeEvent : public Event {
public:
  WindowResizeEvent(uint width, uint height) : width_(width), height_(height) {}

  [[nodiscard]] inline uint GetWidth() const { return width_; }
  [[nodiscard]] inline uint GetHeigth() const { return height_; }

  [[nodiscard]] std::string ToString() const override {
    std::stringstream ss;
    ss << "WindowResize: " << width_ << ", " << height_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(WindowResize)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
  uint width_;
  uint height_;
};

class Slash_API WindowFocusEvent : public Event {
public:
  EVENT_CLASS_TYPE(WindowFocus)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class Slash_API WindowMovedEvent : public Event {
public:
  EVENT_CLASS_TYPE(WindowMoved)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class Slash_API AppTickEvent : public Event {
public:
  EVENT_CLASS_TYPE(AppTick)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class Slash_API AppUpdateEvent : public Event {
public:
  EVENT_CLASS_TYPE(AppUpdate)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class Slash_API AppRenderEvent : public Event {
public:
  EVENT_CLASS_TYPE(AppRender)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

} // namespace slash

#endif // SLASHENGINE_EVENTS_APPLICATION_EVENT_H_