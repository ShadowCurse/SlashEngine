#include "slash_pch.hpp"
#include "layer_stack.hpp"

namespace slash {

LayerStack::~LayerStack() {
  for (Layer *layer : layers_) {
    layer->OnDetach();
    delete layer;
  }
}

void LayerStack::PushLayer(Layer *layer) {
  layers_.emplace(layers_.begin() + layer_insert_index_, layer);
  ++layer_insert_index_;
  layer->OnAttach();
}

void LayerStack::PushOverlay(Layer *overlay) {
  layers_.emplace_back(overlay);
  overlay->OnAttach();
}

void LayerStack::PopLayer(Layer *layer) {
  auto it =
      std::find(layers_.begin(), layers_.begin() + layer_insert_index_, layer);
  if (it != layers_.begin() + layer_insert_index_) {
    layer->OnDetach();
    layers_.erase(it);
    --layer_insert_index_;
  }
}

void LayerStack::PopOverlay(Layer *overlay) {
  auto it =
      std::find(layers_.begin() + layer_insert_index_, layers_.end(), overlay);
  if (it != layers_.end()) {
    overlay->OnDetach();
    layers_.erase(it);
  }
}

} // namespace slash