#ifndef SLASHENGINE_SRC_CORE_MODULES_HPP_
#define SLASHENGINE_SRC_CORE_MODULES_HPP_

#include "Core/core.hpp"

namespace slash {

template <typename ... Types>
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
//  virtual void uninit() = 0;
};

template <typename M>
class NewModule final : public Module {
 public:
  template<typename ... Args>
  explicit NewModule(class App& app, Args&& ... args) : module_(app, std::forward<Args>(args)...) {}
  ~NewModule() final = default;
 private:
  M module_;
};

}

#endif //SLASHENGINE_SRC_CORE_MODULES_HPP_
