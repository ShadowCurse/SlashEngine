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
  template <typename API> static void Create() {
    api_instance_ = CreateUnique<API>();
  }
  template <typename API> static API &GetAPI() {
    return *static_cast<API *>(api_instance_.get());
  }
  static void AddWindow(std::shared_ptr<Window> window);
  static void Init();
  static void Destroy();
  static void DrawFrame(double time);
  static void UpdateScene();

  static void BindModel(std::shared_ptr<Model> model);
  static void UnBindModel(std::shared_ptr<Model> model);

  static void BindMesh(std::shared_ptr<Mesh_3D> mesh);
  static void UnBindMesh(std::shared_ptr<Mesh_3D> mesh);

  static void BindTexture(std::shared_ptr<Texture> texture);
  static void UnBindTexture(std::shared_ptr<Texture> texture);

private:
  inline static Unique<RendererAPI> api_instance_ = nullptr;
};

} // namespace slash

#endif // SLASHENGINE_RENDERER_RENDERER_H_