#include "render_module.hpp"
#include "VulkanRenderer/vulkan_renderer.hpp"
#include "VulkanResources/vulkan_resource_manager.hpp"

namespace slash {

void RenderModule::Init(Shared<Window> window) {
  instance_ = new RenderModule(std::move(window), RenderType::None);
  instance_->renderer_ = nullptr;
  instance_->resource_manager_ = nullptr;
}

void RenderModule::Init(Shared<Window> window, RenderType render_type) {
  instance_ = new RenderModule(std::move(window), render_type);
  switch (render_type) {
  case RenderType::None: {
    instance_->renderer_ = nullptr;
    instance_->resource_manager_ = nullptr;
    break;
  }
  case RenderType::Vulkan: {
    instance_->renderer_ = new VulkanRenderer(instance_->window_.get());
    instance_->resource_manager_ = new VulkanResourceManager(instance_->renderer_);
    break;
  }
  default:
    throw std::runtime_error("invalid render type");
  }
}

void RenderModule::SetRenderer(RenderType render_type) {
//  if (!instance_)
//    return;
//  switch (render_type) {
//  case RenderType::None: {
//    instance_->renderer_ = nullptr;
//    instance_->resource_manager_ = nullptr;
//    break;
//  }
//  case RenderType::Vulkan: {
//    instance_->renderer_ = nullptr;
//    instance_->resource_manager_ = nullptr;
//    break;
//  }
//  default:
//    throw std::runtime_error("invalid render type");
//  }
}

RenderType RenderModule::GetRenderType() { return instance_->render_type_; }

RenderModule::RenderModule(Shared<Window> window, RenderType render_type)
    : window_(window), render_type_(render_type) {}

RenderModule::~RenderModule() {
  delete renderer_;
  delete resource_manager_;
}

} // namespace slash
