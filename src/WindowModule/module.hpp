#ifndef SLASHENGINE_SRC_WINDOWMODULE_MODULE_HPP_
#define SLASHENGINE_SRC_WINDOWMODULE_MODULE_HPP_

#include "Core/app.hpp"
#include "window_manager.hpp"
#include "events.hpp"

namespace slash {

class WindowModule {
 public:
  WindowModule(App& app, WindowParams params) {
    app.add_resource<WindowManager>(app);

    app.add_event<MouseMovedEvent>();
    app.add_event<MouseScrolledEvent>();
    app.add_event<MouseButtonPressedEvent>();
    app.add_event<MouseButtonReleasedEvent>();
    app.add_event<KeyPressedEvent>();
    app.add_event<KeyReleasedEvent>();
    app.add_event<KeyTypedEvent>();

    auto &window_manager = app.get_resource<WindowManager>();
    window_manager.add_window(std::move(params));

//    app.set_runner([&] { window_manager.run(); });
  }
};

}

#endif //SLASHENGINE_SRC_WINDOWMODULE_MODULE_HPP_
