#ifndef SLASHENGINE_SRC_CORE_MODULES_HPP_
#define SLASHENGINE_SRC_CORE_MODULES_HPP_

namespace slash {

template<typename ... T>
struct AppBuilder;

template<typename ... T>
struct NewApp;

template<typename ... T>
struct Dependencies {
  using dependencies = Dependencies<T...>;
  template<typename ... TT>
  [[nodiscard]] static constexpr auto check_deps() -> bool {
    return AppBuilder<TT...>::template check_deps<T...>();
  }
};

//TODO solve this
template<typename T>
//concept IsModule = std::is_class_v<typename T::deps> && std::is_base_of_v<Module2, T>;
concept IsModule = requires(T t) {
  typename T::dependencies;
//  {t.init(TT{}...)} -> std::same_as<bool>;
//  {t.destroy()} -> std::same_as<bool>;
};// std::is_class_v<typename T::dependencies> && std::is_function_v<typeof(typename T::init)> && std::is_same_v<typeof(typename T::init()), bool>;

}

#endif //SLASHENGINE_SRC_CORE_MODULES_HPP_
