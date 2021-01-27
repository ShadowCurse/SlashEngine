#ifndef SLASHENGINE_SRC_CORE_MODULES_HPP_
#define SLASHENGINE_SRC_CORE_MODULES_HPP_

#include "Core/core.hpp"

namespace slash {

template<typename ... Types>
struct Modules {
  template<class Type>
  constexpr bool has() {
    return (std::is_same_v<Type, Types> || ...);
  }
};

class Module {
 public:
  virtual ~Module() = default;
//  virtual void init() = 0;
  virtual void remove(class App &) = 0;
};

template<typename M>
class NewModule final : public Module {
 public:
  using init_fn = std::function<decltype(M::init)>;
  using remove_fn = std::function<decltype(M::remove)>;

 public:
  template<typename ... Args>
  explicit NewModule(class App &app, Args &&... args)
      : init_{M::init},
        remove_{M::remove} {
    init_(app, std::forward<Args>(args)...);
  }
  ~NewModule() final = default;
  void remove(App &app) final {
    remove_(app);
  }
 private:
  init_fn init_;
  remove_fn remove_;

};

}

#endif //SLASHENGINE_SRC_CORE_MODULES_HPP_
