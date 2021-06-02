#ifndef SLASHENGINE_CORE_APPLICATION_H_
#define SLASHENGINE_CORE_APPLICATION_H_

#include "event.hpp"
#include "core.hpp"
#include "modules.hpp"
#include "resource.hpp"
#include "events.hpp"
#include "systems.hpp"
#include "Ecs/ecs.hpp"
#include "GameResources/mesh.hpp"

namespace slash {

class Slash_API App {
 public:
  using runner_fn = std::function<void()>;

 public:
  App() {
    slash::Log::Init();
    add_event<AppClose>();
    add_event<AddMesh>();
    get_event<AppClose>().subscribe([&](auto) { on_close(); });
  }

  ~App() {
    std::for_each(std::rbegin(modules_), std::rend(modules_), [this](auto &module) {
      module->remove(*this);
    });
  }

  void run() {
    SL_CORE_INFO("App start");

    while (run_)
      update();

    SL_CORE_INFO("App shutdown");
  }

  void update() {
    for (const auto &sys: systems_)
      if (run_) std::invoke(*sys, *this);
      else return;
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

  template<typename R>
  void remove_resource() {
    return resource_pack_.remove_resource<R>();
  }

  template<typename E>
  void add_event() {
    event_pool_.template add_event<E>();
  }

  template<typename E>
  auto get_event() -> Event<E> & {
    return event_pool_.get_event<E>();
  }

  template<typename C>
  void init_component() {
    ecs_.register_component<C>();
  }

  void add_system(const std::function<void(App &)> &system) {
    systems_.emplace_back(std::make_unique<System>(system));
  }

  template<typename C>
  void register_component() {
    ecs_.template register_component<C>();
  }

  auto create_entity() {
    return ecs_.create_entity();
  }

  template<typename C>
  void add_component(Entity e, C&& component) {
    ecs_.add_component(e, std::forward<C>(component));
  }

  void add_mesh(Entity e, Mesh&& mesh) {
    ecs_.add_component(e, std::forward<Mesh>(mesh));
    get_event<AddMesh>().template emit(e);
  }

  template<typename C>
  void remove_component(Entity e) {
    ecs_.remove_component<C>(e);
  }

  template<typename C>
  auto get_component(Entity e) -> C & {
    return ecs_.get_component<C>(e);
  }

  template<typename ... C>
  [[nodiscard]] auto get_component_query() {
    return ecs_.get_query<C...>();
  }

 private:
  void on_close() {
    run_ = false;
  }

  bool run_{true};
  ResourcePack resource_pack_;
  EventPool event_pool_;
  std::vector<std::unique_ptr<Module>> modules_;
  std::vector<std::unique_ptr<System>> systems_;
  ECS ecs_;
};

} // namespace slash

#endif // SLASHENGINE_CORE_APPLICATION_H_