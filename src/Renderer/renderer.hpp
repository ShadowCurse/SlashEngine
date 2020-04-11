#ifndef SLASHENGINE_RENDERER_RENDERER_H_
#define SLASHENGINE_RENDERER_RENDERER_H_

#include "Core/core.hpp"
#include "Core/window.hpp"
#include "Renderer/vertex.hpp"
#include "renderer_api.hpp"

namespace slash {

class Slash_API Renderer {
 private:
  Renderer() = default;

 public:
  virtual ~Renderer() = default;
  template<typename API>
  static void Create() {
    api_instance_ = CreateUnique<API>();
  }
  template<typename API>
  static API &GetAPI() {
    return *static_cast<API *>(api_instance_.get());
  }
  static void AddWindow(std::shared_ptr<Window> window);
  static void Init();
  static void Destroy();
  static void DrawFrame(float time);
  static void UpdateScene();
  static void BindVertexBuffer(size_t uid, const std::vector<Vertex> &vertices);
  static void BindIndexBuffer(size_t uid, const std::vector<uint16_t> &indices);
  static void UnBindVertexBuffer(size_t uid);
  static void UnBindIndexBuffer(size_t uid);

  static void BindTexture(size_t uid, const Texture &texture);
  static void UnBindTexture(size_t uid);

  static void BindObject(const ObjectInfo &object_info);

 private:
  inline static Unique<RendererAPI> api_instance_ = nullptr;
};

} // namespace slash

#endif // SLASHENGINE_RENDERER_RENDERER_H_