#ifndef SLASHENGINE_CORE_LAYER_STACK_H_
#define SLASHENGINE_CORE_LAYER_STACK_H_

#include "slash_pch.hpp"

#include "Core/core.hpp"
#include "layer.hpp"

namespace slash::layer {

class Slash_API LayerStack {
 public:
  using layers = std::vector<std::unique_ptr<Layer>>;
  using iterator = layers::iterator;

 public:
  LayerStack() = default;

  template<typename L>
  void push_layer() {
    layers_.template emplace_back(std::make_unique<L>());
  }
  void pop_layer() {
    layers_.pop_back();
  }

  auto begin() -> iterator { return layers_.begin(); }
  auto end() -> iterator { return layers_.end(); }

 private:
  layers layers_;
};

} // namespace slash

#endif // SLASHENGINE_CORE_LAYER_STACK_H_