#ifndef SLASHENGINE_SRC_ASSETS_MODULE_HPP_
#define SLASHENGINE_SRC_ASSETS_MODULE_HPP_

#include "Core/event.hpp"
#include "Core/resource.hpp"
#include "GameResources/mesh.hpp"
#include "GameResources/texture.hpp"
#include "GameResources/transform.hpp"

namespace slash {

  struct AssetsModule;

  template <typename T>
    struct AssetHandle {};
  template <typename T>
    struct Asset {
      Asset(AssetsModule& am) : am_{am} {}

      auto add(T&& asset) -> AssetHandle<T> {
        am_.ep_.emit<AssetAdd<T>>(std::forawrd<T>(asset));
        return AssetHandle<T>{};
      }

      private:
        AssetsModule& am_;
    };

struct AssetsModule : public Dependencies<ResourcePackModule, EventPoolModule> {
  [[nodiscard]] auto init(ResourcePackModule &rp, EventPoolModule &ep) -> bool {

    rp.add_resource<Asset<Mesh>>(*this);
    /* ecs_ = &ecs; */
    /* ep_ = &ep; */

    return true;
  }

  [[nodiscard]] auto destroy(ResourcePackModule &rp, EventPoolModule &, SystemModule &, ECSModule &ecs) -> bool {

    return true;
  }


 private:
  /* ECSModule* ecs_; */
  /* EventPoolModule* ep_; */
};

}

#endif // SLASHENGINE_SRC_ASSETS_MODULE_HPP_


