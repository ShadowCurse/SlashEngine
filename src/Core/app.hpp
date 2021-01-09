#ifndef SLASHENGINE_CORE_APPLICATION_H_
#define SLASHENGINE_CORE_APPLICATION_H_

#include "Events/event.hpp"
#include "core.hpp"
#include "modules.hpp"
#include "resource.hpp"
#include "events.hpp"

namespace slash {

class Slash_API App {
 public:
  using runner_fn = std::function<void()>;

 public:
  App() {
    slash::Log::Init();
    add_event<AppClose>();
    get_event<AppClose>().subscribe([&](auto){ on_close(); });
  }

  void run() {
    SL_CORE_INFO("App start");

    while (run_)
      update();
//      runner_();

    SL_CORE_INFO("App shutdown");
  }

  void update() {
    for (const auto& sys: systems_) sys();
  }

  template<typename M, typename ... Args>
  auto init_module(Args &&... args) -> App & {
    modules_.push_back(std::make_unique<NewModule<M>>(*this, std::forward<Args>(args)...));
    return *this;
  }

  template<typename R, typename ... Args>
  void add_resource(Args &&... args) {
    resource_pack_.add_resource<R>(std::forward<Args>(args)...);
  }

  template<typename R>
  auto get_resource() -> R & {
    return resource_pack_.get_resource<R>();
  }

  template<typename E>
  void add_event() {
    event_pool_.template add_event<E>();
  }

  template<typename E>
  auto get_event() -> Event<E> & {
    return event_pool_.get_event<E>();
  }

  void add_system(const std::function<void()>& system) {
    systems_.emplace_back(system);
  }

//  void set_runner(const runner_fn &runner) {
//    runner_ = runner;
//  }

 private:

  void on_close() {
    run_ = false;
  }

  bool run_{true};
//  runner_fn runner_ = [this]() { update(); };
  ResourcePack resource_pack_;
  EventPool event_pool_;
  std::vector<std::unique_ptr<Module>> modules_;
  std::vector<std::function<void()>> systems_;
};

} // namespace slash

#endif // SLASHENGINE_CORE_APPLICATION_H_