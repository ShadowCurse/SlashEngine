#ifndef SLASHENGINE_SRC_RENDERER_IMGUI_LAYER_HPP_
#define SLASHENGINE_SRC_RENDERER_IMGUI_LAYER_HPP_

#include "LayerModule/layer.hpp"
#include "VulkanRenderer/vulkan_imgui.hpp"

namespace slash {

class ImGuiOverlay final : public Layer {
 public:
  ImGuiOverlay(slash::ImGuiDrawFn &fn);
  ~ImGuiOverlay() final;

  void SetVisibility(bool visible);

 private:
};

} // namespace slash
#endif // SLASHENGINE_SRC_RENDERER_IMGUI_LAYER_HPP_
