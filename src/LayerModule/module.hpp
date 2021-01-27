#ifndef SLASHENGINE_SRC_LAYERMODULE_LAYERMODULE_HPP_
#define SLASHENGINE_SRC_LAYERMODULE_LAYERMODULE_HPP_

#include "Core/app.hpp"

#include "WindowModule/module.hpp"
#include "layer_stack.hpp"

namespace slash {

class LayerModule {
 public:
  explicit LayerModule(App &app) {
    app.add_resource<LayerStack>();

//    auto &layer_stack = app.get_resource<LayerStack>();

//    for (const auto &window: app.get_resource<WindowManager>().get_windows()) {
//      window->set_event_callback([&](Event &e) { layer_stack.on_event(e); });
//    }
  }
};

}

#endif //SLASHENGINE_SRC_LAYERMODULE_LAYERMODULE_HPP_
