#ifndef SLASHENGINE_RENDERER_RENDERERAPI_H_
#define SLASHENGINE_RENDERER_RENDERERAPI_H_

#include "Core/core.hpp"
#include "Core/window.hpp"
#include "GameResources/model.hpp"
#include "GameResources/texture.hpp"
#include "Renderer/vertex.hpp"

namespace slash {
class Slash_API RendererAPI {
protected:
  RendererAPI() = default;

public:
  virtual ~RendererAPI() = default;
  virtual void AddWindow(std::shared_ptr<Window> window) = 0;
  virtual void Init() = 0;
  virtual void Destroy() = 0;
  virtual void DrawFrame(float time) = 0;
  virtual void UpdateScene() = 0;

  virtual void BindModel(std::shared_ptr<Model> model) = 0;
  virtual void UnBindModel(std::shared_ptr<Model> model) = 0;

  virtual void BindMesh(std::shared_ptr<Mesh_3D> mesh) = 0;
  virtual void UnBindMesh(std::shared_ptr<Mesh_3D> mesh) = 0;

  virtual void BindTexture(std::shared_ptr<Texture> texture) = 0;
  virtual void UnBindTexture(std::shared_ptr<Texture> texture) = 0;
};

} // namespace slash

#endif // SLASHENGINE_RENDERER_RENDERERAPI_H_