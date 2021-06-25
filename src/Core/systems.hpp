#ifndef SLASHENGINE_SRC_CORE_SYSTEMS_HPP_
#define SLASHENGINE_SRC_CORE_SYSTEMS_HPP_

#include "log.hpp"
#include "modules.hpp"

namespace slash {

class System {
 public:
  using system_type = std::function<void()>;

 public:
  explicit System(system_type system) : system_{std::move(system)} {}
  ~System() = default;
  void operator()() {
    system_();
  }
 private:
  system_type system_;
};

class SystemModule : public Dependencies<EventPoolModule> {
 public:
  SystemModule() = default;
  [[nodiscard]] auto init(EventPoolModule &ep) -> bool {
    ep.get_event<AppClose>().subscribe([&](auto) { on_close(); });
    return true;
  }

  [[nodiscard]] auto destroy(EventPoolModule &) -> bool {
    return true;
  }

  void run() {
    SL_CORE_INFO("App start");

    while (run_)
      update();

    SL_CORE_INFO("App shutdown");
  }

  void update() {
    for (const auto &sys: systems_)
      if (run_) std::invoke(*sys);
      else return;
  }

  void add_system(const std::function<void()> &system) {
    systems_.emplace_back(std::make_unique<System>(system));
  }

  void on_close() {
    run_ = false;
  }

 private:
  bool run_{true};
  std::vector<std::unique_ptr<System>> systems_;
};

}

#endif //SLASHENGINE_SRC_CORE_SYSTEMS_HPP_
