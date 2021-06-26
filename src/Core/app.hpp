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

} // namespace slash

#endif // SLASHENGINE_CORE_APPLICATION_H_