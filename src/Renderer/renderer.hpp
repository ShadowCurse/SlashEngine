#ifndef SLASHENGINE_RENDERER_RENDERER_H_
#define SLASHENGINE_RENDERER_RENDERER_H_

#include "Core/window.hpp"
#include "renderer_api.hpp"

namespace slash {

class Renderer {
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
  static void DrawFrame(float time);

private:
  inline static Unique<RendererAPI> api_instance_ = nullptr;
};

} // namespace slash

#endif // SLASHENGINE_RENDERER_RENDERER_H_