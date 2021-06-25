#ifndef SLASHENGINE_CORE_EVENT_H_
#define SLASHENGINE_CORE_EVENT_H_

#include "modules.hpp"
#include "events.hpp"

namespace slash {

struct BasicEvent {};

template<typename E>
class Event : public BasicEvent {
 public:
  using subscribe_fn = std::function<void(E &)>;

 public:
  Event() = default;

  void subscribe(const subscribe_fn &fn) {
    subscribers_.emplace_back(fn);
  }

  template<typename ... Args>
//  requires std::is_constructible_v<E, Args...>
  void emit(Args &&... args) {
    E event{std::forward<Args>(args)...};
    for (const auto &sub : subscribers_)
      sub(event);
  }
 private:
  std::vector<subscribe_fn> subscribers_;
};

class EventPoolModule : public Dependencies<> {
 public:
  EventPoolModule() = default;
  auto init() -> bool {
    add_event<AppClose>();
    return true;
  }

  auto destroy() -> bool {
    return true;
  }

  template<typename E>
  void add_event() {
    constexpr auto &info = typeid(E);
    assert(events_.find(info.name()) == std::end(events_));
    events_.insert(std::make_pair(info.name(), std::make_unique<Event<E>>()));
  }
  template<typename E>
  auto get_event() -> Event<E> & {
    constexpr auto &info = typeid(E);
    assert(events_.find(info.name()) != std::end(events_));
    return *static_cast<Event<E> *>(events_[info.name()].get());
  }

 private:
  std::unordered_map<const char *, std::unique_ptr<BasicEvent>> events_;
};

} // namespace slash

#endif // SLASHENGINE_EVENT_EVENT_H_