#ifndef SLASHENGINE_EVENT_EVENT_H_
#define SLASHENGINE_EVENT_EVENT_H_

#include "Core/core.hpp"

//#include "mouse_event.hpp"

namespace slash {

class BasicEvent {
 public:
 virtual ~BasicEvent() = default;
};


template <typename E>
class Event : public BasicEvent {
 public:
  using subscribe_fn = std::function<void(E&)>;

 public:
  Event() = default;

  void subscribe(const subscribe_fn& fn) {
    subscribers_.emplace_back(fn);
  }

  template<typename ... Args>
//  requires std::is_constructible_v<E, Args...>
  void emit(Args&& ... args) {
    E event{std::forward<Args>(args)...};
    for (const auto& sub : subscribers_)
      sub(event);
  }
 private:
  std::vector<subscribe_fn> subscribers_;
};

class EventPool {
 public:
  EventPool() = default;

  template<typename E>
  void add_event() {
    constexpr auto& info = typeid(E);
    assert(events_.find(info.name()) == std::end(events_));
    events_.insert(std::make_pair(info.name(), std::make_unique<Event<E>>()));
  }
  template<typename E>
  auto get_event() -> Event<E>& {
    constexpr auto& info = typeid(E);
    assert(events_.find(info.name()) != std::end(events_));
    return *static_cast<Event<E>*>(events_[info.name()].get());
  }

 private:
  std::unordered_map<const char*, std::unique_ptr<BasicEvent>> events_;
};

//enum class EventType {
//  None = 0,
//  WindowClose,
//  WindowResize,
//  WindowFocus,
//  WindowMoved,
//  KeyPressed,
//  KeyReleased,
//  KeyTyped,
//  MouseButtonPressed,
//  MouseButtonReleased,
//  MouseMoved,
//  MouseScrolled,
//};
//
//enum EventCategory {
//  None = 0,
//  EventCategoryApplication = 1 << 0,
//  EventCategoryInput = 1 << 1,
//  EventCategoryKeyboard = 1 << 2,
//  EventCategoryMouse = 1 << 3,
//  EventCategoryMouseButton = 1 << 4,
//  EventCategoryMouseScroll = 1 << 5,
//};
//
//class Slash_API Event {
// public:
//  virtual ~Event() = default;
//  bool Handled = false;
//
//  [[nodiscard]] virtual auto GetEventType() const -> EventType = 0;
//  [[nodiscard]] virtual auto GetName() const -> const char * = 0; // only for debug
//  [[nodiscard]] virtual auto GetCategoryFlags() const -> int = 0;
//  [[nodiscard]] virtual auto ToString() const -> std::string { return GetName(); }
//  [[nodiscard]] auto IsInCategory(EventCategory category) -> bool {
//    return GetCategoryFlags() & category;
//  }
//};
//
//#define EVENT_CLASS_TYPE(type)                                                 \
//  static auto GetStaticType() ->EventType { return EventType::type; }                 \
//  virtual auto GetEventType() const -> EventType override { return GetStaticType(); }  \
//  virtual auto GetName() const -> const char * override { return #type; }
//
//#define EVENT_CLASS_CATEGORY(category)                                         \
//  virtual auto GetCategoryFlags() const -> int override { return category; }
//
//class Slash_API EventDispatcher {
//  template<typename T> using EventFn = std::function<bool(T &)>;
//
// public:
//  explicit EventDispatcher(Event &event) : event_(event) {}
//
//  template<typename T>
//  auto Dispatch(EventFn<T> func) -> bool {
//    if (event_.GetEventType() == T::GetStaticType()) {
//      event_.Handled = func(static_cast<T &>(event_));
//      return true;
//    }
//    return false;
//  }
//
// private:
//  Event &event_;
//};

} // namespace slash

#endif // SLASHENGINE_EVENT_EVENT_H_