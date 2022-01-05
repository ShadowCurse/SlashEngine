#ifndef SLASHENGINE_SRC_ASSETS_MODULE_HPP_
#define SLASHENGINE_SRC_ASSETS_MODULE_HPP_

#include "Core/event.hpp"
#include "Core/resource.hpp"
#include "GameResources/mesh.hpp"
#include "GameResources/texture.hpp"
#include "GameResources/transform.hpp"

namespace slash {

template <typename T>
struct AssetHandle {};
template <typename T>
struct Asset;

struct AssetsModule : public Dependencies<ResourcePackModule, EventPoolModule> {
  [[nodiscard]] auto init(ResourcePackModule& rp, EventPoolModule& ep) -> bool {
    rp.add_resource<Asset<Mesh>>(*this);
    /* ecs_ = &ecs; */
    /* ep_ = &ep; */

    return true;
  }

  [[nodiscard]] auto destroy(ResourcePackModule& rp, EventPoolModule&) -> bool {
    return true;
  }

  template<typename T> [[nodiscard]] auto add(T&& asset) -> AssetHandle<T> {}
  template<typename T> [[nodiscard]] auto get(AssetHandle<T> handle) -> const T& {}
  template<typename T> [[nodiscard]] auto get_mut(AssetHandle<T> handle) -> T& {}
  template<typename T> void remove(AssetHandle<T> handle) {}

 private:
  /* ECSModule* ecs_; */
  /* EventPoolModule* ep_; */
};

template <typename T>
struct Asset {
  Asset(AssetsModule& am) : am_{am} {}

  [[nodiscard]] auto add(T&& asset) -> AssetHandle<T> {
        return am_.add<T>(std::forward<T>(asset));
  }
  [[nodiscard]] auto get(AssetHandle<T> handle) -> const T& { return am_.get<T>(handle); }
  [[nodiscard]] auto get_mut(AssetHandle<T> handle) -> T& { return am_.get_mut<T>(handle); }
  void remove(AssetHandle<T> handle) { am_.remove<T>(handle); }

 private:
  AssetsModule& am_;
};
}  // namespace slash

#endif  // SLASHENGINE_SRC_ASSETS_MODULE_HPP_
