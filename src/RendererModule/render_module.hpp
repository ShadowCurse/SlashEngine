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

class RenderModule {
public:
  static void Init(Shared<Window> window);
  static void Init(Shared<Window> window, RenderType render_type);

  static void SetRenderer(RenderType render_type);
  static  RenderType GetRenderType();

  static VulkanRenderer* Renderer() { return instance_->renderer_; }
  static VulkanResourceManager* ResourceManager() { return instance_->resource_manager_; }

private:
  RenderModule(Shared<Window> window, RenderType render_type);
  ~RenderModule();
  inline static RenderModule* instance_ = nullptr;

  Shared<Window> window_;
  RenderType render_type_;

  VulkanRenderer* renderer_;
  VulkanResourceManager* resource_manager_;
};

}

#endif // SLASHENGINE_SRC_RENDERERMODULE_RENDER_MODULE_HPP_
