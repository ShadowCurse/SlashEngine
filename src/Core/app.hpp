#ifndef SLASHENGINE_CORE_APPLICATION_H_
#define SLASHENGINE_CORE_APPLICATION_H_

#include "event.hpp"
#include "modules.hpp"
#include "events.hpp"
#include "systems.hpp"
#include "Ecs/ecs.hpp"
#include "GameResources/pack.hpp"

namespace slash {

template<typename ... T>
struct AppBuilder {
  template<IsModule N>
  [[nodiscard]] constexpr auto add_module() const {
    static_assert(N::template check_deps<T...>(), "dependencies are not satisfied");
    return AppBuilder<T..., N>{};
  }

  [[nodiscard]] constexpr auto build() -> NewApp<T...> {
    return NewApp<T...>{};
  }

  template<typename ... TT>
  [[nodiscard]] static constexpr auto check_deps() -> bool {
    return (has_type<TT>() && ... && true);
  }

  template<typename N>
  [[nodiscard]] static constexpr auto has_type() -> bool {
    return (std::is_same_v<T, N> || ... || false);
  }
};

template<typename ... T>
struct NewApp final : public T ... {
  auto init() -> bool {
    return (init_module<T>(typename T::dependencies{}) && ...);
  }

  auto destroy() -> bool {
    std::deque<std::function<bool()>> q;
    (q.push_front([&] { return destroy < T > (); }), ...);
    return std::accumulate(std::begin(q),
                           std::end(q),
                           true,
                           [](auto r, auto &f) {
                             return r && std::invoke(f);
                           });
  }

 private:
  template<typename M, typename ... D>
  auto init_module(Dependencies<D...>) -> bool {
    return M::init(*static_cast<D *>(this)...);
  }

  template<typename M>
  auto destroy() -> bool {
    return destroy_module<M>(typename M::dependencies{});
  }

  template<typename M, typename ... D>
  auto destroy_module(Dependencies<D...>) -> bool {
    return M::destroy(*static_cast<D *>(this)...);
  }
};

//class Slash_API App {
// public:
//  using runner_fn = std::function<void()>;
//
// public:
//  App() {
//    slash::Log::Init();
//    add_event<AppClose>();
//    add_event<CreateRenderable>();
//    get_event<AppClose>().subscribe([&](auto) { on_close(); });
//  }
//
//  ~App() {
//    std::for_each(std::rbegin(modules_), std::rend(modules_), [this](auto &module) {
//      module->remove(*this);
//    });
//    SL_CORE_INFO("App destroyed");
//  }
//
//  void run() {
//    SL_CORE_INFO("App start");
//
//    while (run_)
//      update();
//
//    SL_CORE_INFO("App shutdown");
//  }
//
//  void update() {
//    for (const auto &sys: systems_)
//      if (run_) std::invoke(*sys, *this);
//      else return;
//  }
//
//  template<typename M, typename ... Args>
//  auto init_module(Args &&... args) -> App & {
//    modules_.push_back(std::make_unique<NewModule<M>>(*this, std::forward<Args>(args)...));
//    return *this;
//  }
//
//  template<typename R, typename ... Args>
//  void add_resource(Args &&... args) {
//    resource_pack_.add_resource<R>(std::forward<Args>(args)...);
//  }
//
//  template<typename R>
//  auto get_resource() -> R & {
//    return resource_pack_.get_resource<R>();
//  }
//
//  template<typename R>
//  void remove_resource() {
//    return resource_pack_.remove_resource<R>();
//  }
//
//  template<typename E>
//  void add_event() {
//    event_pool_.template add_event<E>();
//  }
//
//  template<typename E>
//  auto get_event() -> Event<E> & {
//    return event_pool_.get_event<E>();
//  }
//
//  template<typename C>
//  void init_component() {
//    ecs_.register_component<C>();
//  }
//
//  void add_system(const std::function<void(App &)> &system) {
//    systems_.emplace_back(std::make_unique<System>(system));
//  }
//
//  template<typename C>
//  void register_component() {
//    ecs_.template register_component<C>();
//  }
//
//  template<typename C>
//  void unregister_component() {
//    ecs_.template unregister_component<C>();
//  }
//
//  auto create_entity() {
//    return ecs_.create_entity();
//  }
//
//  template<typename C>
//  void add_component(Entity e, C &&component) {
//    ecs_.add_component(e, std::forward<C>(component));
//  }
//
//  auto add_pack(PackObject3d &&pack) -> Entity {
//    auto e = create_entity();
//    ecs_.add_component(e, std::forward<PackObject3d>(pack));
//    get_event<CreateRenderable>().template emit(e);
//    return e;
//  }
//
//  template<typename C>
//  void remove_component(Entity e) {
//    ecs_.remove_component<C>(e);
//  }
//
//  template<typename C>
//  auto get_component(Entity e) -> C & {
//    return ecs_.get_component<C>(e);
//  }
//
//  template<typename ... C>
//  [[nodiscard]] auto get_component_query() {
//    return ecs_.get_query<C...>();
//  }
//
// private:
//  void on_close() {
//    run_ = false;
//  }
//
//  bool run_{true};
//  ResourcePack resource_pack_;
//  EventPool event_pool_;
//  std::vector<std::unique_ptr<Module>> modules_;
//  std::vector<std::unique_ptr<System>> systems_;
//  ECS ecs_;
//};

} // namespace slash

#endif // SLASHENGINE_CORE_APPLICATION_H_