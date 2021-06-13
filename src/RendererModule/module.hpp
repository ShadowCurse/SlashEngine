#ifndef SLASHENGINE_SRC_RENDERERMODULE_MODULE_HPP_
#define SLASHENGINE_SRC_RENDERERMODULE_MODULE_HPP_

#include "Core/core.hpp"
#include "WindowModule/module.hpp"
#include "VulkanRenderer/vulkan_renderer.hpp"
#include "VulkanResources/vulkan_resource_manager.hpp"

namespace slash {

class Slash_API RenderModule {
 public:
  static void init(App &app) {

    app.register_component<VulkanRenderableObject>();
    app.register_component<Mesh>();
    app.register_component<Texture>();
    app.register_component<Transform>();

    auto &wm = app.get_resource<WindowManager>();
    auto &window = wm.get_windows()[0];

    app.add_resource<VulkanRenderer>(window.get());
    auto &renderer = app.get_resource<VulkanRenderer>();
    app.add_resource<VulkanResourceManager>(&app, &renderer);

    app.add_system([](App &app) {
      auto &renderer = app.get_resource<VulkanRenderer>();
      renderer.new_frame();
      auto render_command = renderer.start_render_command();
      auto query = app.get_component_query<VulkanRenderableObject>();
      for (auto[object]: query)
        render_command->AddRenderableObject(object);
      renderer.end_render_command(render_command);
      renderer.draw_frame(0.0);
    });
  }
  static void remove(App &app) {
    SL_CORE_INFO("RenderModule remove");
    app.get_resource<VulkanRenderer>().wait_idle();
    app.unregister_component<VulkanRenderableObject>();
    app.unregister_component<Mesh>();
    app.unregister_component<Texture>();
    app.unregister_component<Transform>();
    app.remove_resource<VulkanResourceManager>();
    app.remove_resource<VulkanRenderer>();
  }
};

}

#endif // SLASHENGINE_SRC_RENDERERMODULE_MODULE_HPP_
