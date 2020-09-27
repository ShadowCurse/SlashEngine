#ifndef SLASHENGINE_CORE_LAYER_STACK_H_
#define SLASHENGINE_CORE_LAYER_STACK_H_

#include "Core/core.hpp"
#include "layer.hpp"
#include "slash_pch.hpp"

namespace slash {

class Slash_API LayerStack {
 public:
  LayerStack() = default;
  virtual ~LayerStack();

  void PushLayer(Layer *layer);
  void PushOverlay(Layer *layer);
  void PopLayer(Layer *layer);
  void PopOverlay(Layer *layer);

  auto begin() -> std::vector<Layer *>::iterator { return layers_.begin(); }
  auto end() -> std::vector<Layer *>::iterator { return layers_.end(); }

 private:
  std::vector<Layer *> layers_;
  uint layer_insert_index_ = 0;
};

} // namespace slash

#endif // SLASHENGINE_CORE_LAYER_STACK_H_