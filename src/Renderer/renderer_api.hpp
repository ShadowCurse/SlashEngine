#ifndef SLASHENGINE_RENDERER_RENDERERAPI_H_
#define SLASHENGINE_RENDERER_RENDERERAPI_H_

#include "Core/window.hpp"
#include "slash_pch.hpp"

namespace slash {
class RendererAPI {
protected:
  RendererAPI() = default;

public:
  virtual ~RendererAPI() = default;
  virtual void AddWindow(std::shared_ptr<Window> window) = 0;
  virtual void Init() = 0;
  virtual void Destroy() = 0;
  virtual void DrawFrame(float time) = 0;
};

} // namespace slash

#endif // SLASHENGINE_RENDERER_RENDERERAPI_H_