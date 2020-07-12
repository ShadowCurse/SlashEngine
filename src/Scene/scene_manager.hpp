#ifndef SLASHENGINE_SRC_SCENE_SCENE_MANAGER_HPP_
#define SLASHENGINE_SRC_SCENE_SCENE_MANAGER_HPP_

#include "Core/core.hpp"
#include "RendererModule/render_module.hpp"

namespace slash {

class SceneManager {
public:
  static void PrepareScene();
  static void DrawScene(double dt);
  static void AddToScene(const std::string& object_name);
};

}

#endif // SLASHENGINE_SRC_SCENE_SCENE_MANAGER_HPP_
