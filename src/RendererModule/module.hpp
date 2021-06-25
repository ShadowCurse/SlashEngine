#ifndef SLASHENGINE_SRC_RENDERERMODULE_MODULE_HPP_
#define SLASHENGINE_SRC_RENDERERMODULE_MODULE_HPP_

#include "events.hpp"
#include "WindowModule/module.hpp"
#include "VulkanRenderer/vulkan_renderer.hpp"
#include "VulkanResources/vulkan_resource_manager.hpp"

namespace slash {

struct RenderModule : public Dependencies<ResourcePackModule, EventPoolModule, SystemModule, ECSModule> {
  [[nodiscard]] auto init(ResourcePackModule &rp, EventPoolModule &ep, SystemModule &sm, ECSModule &ecs) -> bool {
    ep.add_event<CreateRenderable>();

    ecs.register_component<VulkanRenderableObject>();
    ecs.register_component<Mesh>();
    ecs.register_component<Texture>();
    ecs.register_component<Transform>();
    ecs.register_component<PackObject3d>();

    auto &wmgr = rp.get_resource<WindowManager>();
    auto &window = wmgr.get_windows()[0];

    rp.add_resource<VulkanRenderer>(window.get());
    auto &renderer = rp.get_resource<VulkanRenderer>();
    rp.add_resource<VulkanResourceManager>(ep, ecs, &renderer);

    sm.add_system([&] {
      auto &renderer = rp.get_resource<VulkanRenderer>();
      renderer.new_frame();
      auto render_command = renderer.start_render_command();
      auto query = ecs.get_query<VulkanRenderableObject>();
      for (auto[object]: query)
        render_command->AddRenderableObject(object);
      renderer.end_render_command(render_command);
      renderer.draw_frame(0.0);
    });
    return true;
  }

  [[nodiscard]] auto destroy(ResourcePackModule &rp, EventPoolModule &, SystemModule &, ECSModule &ecs) -> bool {
    rp.get_resource<VulkanRenderer>().wait_idle();
    ecs.unregister_component<VulkanRenderableObject>();
    ecs.unregister_component<Mesh>();
    ecs.unregister_component<Texture>();
    ecs.unregister_component<Transform>();
    rp.remove_resource<VulkanResourceManager>();
    rp.remove_resource<VulkanRenderer>();
    return true;
  }
};

}

#endif // SLASHENGINE_SRC_RENDERERMODULE_MODULE_HPP_
