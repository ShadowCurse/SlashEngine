#ifndef SLASHENGINE_SRC_CORE_RESOURCE_HPP_
#define SLASHENGINE_SRC_CORE_RESOURCE_HPP_

#include "Core/core.hpp"
#include "Core/log.hpp"

namespace slash {

struct Resource {};

template<typename R>
class NewResource : public Resource {
 public:
  template<typename ... Args>
  explicit NewResource(Args &&... args) : resource_(std::forward<Args>(args)...) {}
  R &get() { return resource_; }
 private:
  R resource_;
};

class ResourcePack {
 public:
  ResourcePack() = default;

  template<typename R, typename ... Args>
  void add_resource(Args &&... args) {
    constexpr auto &id = typeid(R);
    resources_.insert(std::make_pair(id.name(), std::make_unique<NewResource<R>>(std::forward<Args>(args)...)));
  }

  template<typename R>
  auto get_resource() -> R & {
    constexpr auto &id = typeid(R);
    return static_cast<NewResource<R> *>(resources_[id.name()].get())->get();
  }

 private:
  std::unordered_map<const char *, std::unique_ptr<Resource>> resources_;
};

}

#endif //SLASHENGINE_SRC_CORE_RESOURCE_HPP_
