#ifndef SLASHENGINE_SRC_ASSETS_MODULE_HPP_
#define SLASHENGINE_SRC_ASSETS_MODULE_HPP_

#include "Core/event.hpp"
#include "Core/resource.hpp"
#include "Core/systems.hpp"
#include "GameResources/mesh.hpp"
#include "GameResources/texture.hpp"
#include "GameResources/transform.hpp"
#include "assets.hpp"

namespace slash {

struct AssetsModule : public Dependencies<ResourcePackModule, SystemModule> {
  [[nodiscard]] auto init(ResourcePackModule &rp, SystemModule &sm) -> bool {
    rp.add_resource<AssetsModule>(sm);
    auto am = rp.get_resource<AssetsModule>();

    rp.add_resource<Asset<Mesh, VulkanMesh>>(am,
        [&](auto mesh) {
          return vrm.create_mesh(mesh);
        };
    return true;
  }

  [[nodiscard]] auto destroy(ResourcePackModule &rp, EventPoolModule &)
      -> bool {
    return true;
  }
};

} // namespace slash

#endif // SLASHENGINE_SRC_ASSETS_MODULE_HPP_
