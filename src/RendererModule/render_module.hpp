#ifndef SLASHENGINE_SRC_RENDERERMODULE_RENDER_MODULE_HPP_
#define SLASHENGINE_SRC_RENDERERMODULE_RENDER_MODULE_HPP_

#include "Core/core.hpp"
#include "Core/window.hpp"
#include "VulkanRenderer/vulkan_renderer.hpp"
#include "VulkanResources/vulkan_resource_manager.hpp"

namespace slash {

enum class RenderType {
  None, Vulkan
};

class Slash_API RenderModule {
public:
  static void Init(std::shared_ptr<Window> window);
  static void Init(std::shared_ptr<Window> window, RenderType render_type);
  static void Destroy();

  static void SetRenderer(RenderType render_type);
  static auto GetRenderType() -> RenderType;

  static auto Renderer() -> VulkanRenderer& { return *instance_->renderer_; }
  static auto ResourceManager() -> VulkanResourceManager& { return *instance_->resource_manager_; }

private:
  RenderModule(std::shared_ptr<Window> window, RenderType render_type);
  ~RenderModule() = default;
  inline static RenderModule* instance_ = nullptr;

  std::shared_ptr<Window> window_;
  RenderType render_type_;

  std::unique_ptr<VulkanRenderer> renderer_;
  std::unique_ptr<VulkanResourceManager> resource_manager_;
};

}

#endif // SLASHENGINE_SRC_RENDERERMODULE_RENDER_MODULE_HPP_
