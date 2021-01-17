#ifndef SLASHENGINE_SRC_CORE_SYSTEMS_HPP_
#define SLASHENGINE_SRC_CORE_SYSTEMS_HPP_

#include "core.hpp"

namespace slash {

class App;

struct BaseSystem {
  virtual ~BaseSystem() = default;
  virtual void operator()(App& app) = 0;
};

class System {
 public:
  using system_type = std::function<void(App& app)>;

 public:
  explicit System(system_type system) : system_{std::move(system)} {}
  ~System() = default;
  void operator()(App& app) {
    system_(app);
  }
 private:
  system_type system_;
};

//void a() {
//  System s([](Query<int>, int){});
//}

}

#endif //SLASHENGINE_SRC_CORE_SYSTEMS_HPP_
