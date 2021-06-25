#ifndef SLASHENGINE_SRC_WINDOWMODULE_MODULE_HPP_
#define SLASHENGINE_SRC_WINDOWMODULE_MODULE_HPP_

#include "Core/modules.hpp"
#include "window_manager.hpp"
#include "events.hpp"

namespace slash {

struct WindowModule : public Dependencies<ResourcePackModule, EventPoolModule, SystemModule> {
  [[nodiscard]] auto init(ResourcePackModule &rp, EventPoolModule &ep, SystemModule &sm) -> bool {
    rp.add_resource<WindowManager>(ep, sm);

    ep.add_event<MouseMovedEvent>();
    ep.add_event<MouseScrolledEvent>();
    ep.add_event<MouseButtonPressedEvent>();
    ep.add_event<MouseButtonReleasedEvent>();
    ep.add_event<KeyPressedEvent>();
    ep.add_event<KeyReleasedEvent>();
    ep.add_event<KeyTypedEvent>();

    auto &window_manager = rp.get_resource<WindowManager>();
    auto wp = WindowParams("TestApp", 800, 400);
    window_manager.add_window(std::move(wp));
    return true;
  }

  [[nodiscard]] auto destroy(ResourcePackModule &rp, EventPoolModule &, SystemModule &) -> bool {
    SL_CORE_INFO("WindowModule remove");
    rp.remove_resource<WindowManager>();
    return true;
  }
};

}

#endif //SLASHENGINE_SRC_WINDOWMODULE_MODULE_HPP_
