#ifndef SLASHENGINE_SRC_LAYERMODULE_LAYERMODULE_HPP_
#define SLASHENGINE_SRC_LAYERMODULE_LAYERMODULE_HPP_

#include "layer_stack.hpp"
#include "WindowModule/module.hpp"

namespace slash {

class LayerModule {
 public:
  static void init() {
    if (!instance_) {
      SL_CORE_INFO("Initializing LayerModule");
      instance_ = new LayerModule;
//      WindowModule::set_window_callback([](Event& e){ instance_->on_event_callback(e); });
    } else {
      SL_CORE_ERROR("trying to reinitialize LayerModule");
    }
  }
  static void uninit() {
      delete instance_;
  }

  template<typename L>
  static void push_layer() {
    instance_->layer_stack_.push_layer<L>();
  }
  static void pop_layer() {
    instance_->layer_stack_.pop_layer();
  }

  void on_event_callback(Event &e) {
    for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
      (*--it)->on_event(e);
      if (e.Handled)
        break;
    }
  }

 private:
  layer::LayerStack layer_stack_;
  static inline LayerModule* instance_ = nullptr;
};

}

#endif //SLASHENGINE_SRC_LAYERMODULE_LAYERMODULE_HPP_
