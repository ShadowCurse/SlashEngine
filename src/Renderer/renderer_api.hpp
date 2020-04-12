#ifndef SLASHENGINE_RENDERER_RENDERERAPI_H_
#define SLASHENGINE_RENDERER_RENDERERAPI_H_

#include "Core/core.hpp"
#include "Core/window.hpp"
#include "Renderer/vertex.hpp"
#include "GameResources/texture.hpp"
#include "GameResources/object_info.hpp"

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
  virtual void BindModel(size_t uid,
                         const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) = 0;
  virtual void UnBindModel(size_t uid) = 0;

  virtual void BindTexture(size_t uid, const Texture &texture) = 0;
  virtual void UnBindTexture(size_t uid) = 0;

  virtual void BindObject(const ObjectInfo &object_info) = 0;

};

} // namespace slash

#endif // SLASHENGINE_RENDERER_RENDERERAPI_H_