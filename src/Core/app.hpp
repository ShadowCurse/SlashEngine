#ifndef SLASHENGINE_CORE_APPLICATION_H_
#define SLASHENGINE_CORE_APPLICATION_H_

#include <memory>

#include "Events/application_event.hpp"
#include "Events/event.hpp"
#include "core.hpp"
#include "time_step.hpp"
#include "window.hpp"
#include "modules.hpp"
#include "resource.hpp"

namespace slash {

class Slash_API App {
 public:
  using runner_fn = std::function<void()>;

 public:
  App() {
    slash::Log::Init();
  }

  void run() {
    SL_CORE_INFO("App start");

    runner_();

    SL_CORE_INFO("App shutdown");
  }

  void update() {}

  template<typename Module, typename ... Args>
  void init_module(Args &&... arg) {
    Module::init(std::forward<Args>(arg)...);
  }

  template<typename Module>
  void uninit_module() {
    Module::uninit();
  }

  template<typename M, typename ... Args>
  auto init_module_2(Args &&... args) -> App & {
    modules_.push_back(std::make_unique<NewModule<M>>(*this, std::forward<Args>(args)...));
    return *this;
  }

  template<typename R, typename ... Args>
  void add_resource(Args &&... args) {
    resource_pack_.add_resource<R>(std::forward<Args>(args)...);
  }

  template<typename R>
  auto get_resource() -> R& {
    return resource_pack_.get_resource<R>();
  }

  void set_runner(const runner_fn &runner) {
    runner_ = runner;
  }

 private:
  runner_fn runner_ = [this]() { update(); };
  ResourcePack resource_pack_;
  std::vector<std::unique_ptr<Module>> modules_;
};

} // namespace slash

#endif // SLASHENGINE_CORE_APPLICATION_H_