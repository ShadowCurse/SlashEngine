#ifndef SLASHENGINE_SRC_ASSETS_MODULE_HPP_
#define SLASHENGINE_SRC_ASSETS_MODULE_HPP_

#include "Core/event.hpp"
#include "Core/resource.hpp"
#include "assets.hpp"
#include "GameResources/mesh.hpp"
#include "GameResources/texture.hpp"
#include "GameResources/transform.hpp"

namespace slash {

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

  /* template<typename T> [[nodiscard]] auto add(T&& asset) -> AssetHandle<T> {} */
  /* template<typename T> [[nodiscard]] auto get(AssetHandle<T> handle) -> const T& {} */
  /* template<typename T> [[nodiscard]] auto get_mut(AssetHandle<T> handle) -> T& {} */
  /* template<typename T> void remove(AssetHandle<T> handle) {} */
  /* template<typename T, typename M> void register_asset_manager(M& manager) {} */

 private:
  /* ECSModule* ecs_; */
  /* EventPoolModule* ep_; */
};

}  // namespace slash

#endif  // SLASHENGINE_SRC_ASSETS_MODULE_HPP_
