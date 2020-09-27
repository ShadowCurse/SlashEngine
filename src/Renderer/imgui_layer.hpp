#ifndef SLASHENGINE_SRC_RENDERER_IMGUI_LAYER_HPP_
#define SLASHENGINE_SRC_RENDERER_IMGUI_LAYER_HPP_

#include "Core/core.hpp"
#include "Core/layer.hpp"
#include "VulkanRenderer/vulkan_imgui.hpp"

namespace slash {

class Slash_API ImGuiOverlay final : public Layer {
public:
  ImGuiOverlay(slash::ImGuiDrawFn &fn);
  ~ImGuiOverlay() final;

  void SetVisibility(bool visible);

private:
};

} // namespace slash
#endif // SLASHENGINE_SRC_RENDERER_IMGUI_LAYER_HPP_
