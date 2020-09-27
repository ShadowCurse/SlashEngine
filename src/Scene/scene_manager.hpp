#ifndef SLASHENGINE_SRC_SCENE_SCENE_MANAGER_HPP_
#define SLASHENGINE_SRC_SCENE_SCENE_MANAGER_HPP_

#include "Core/core.hpp"
#include "scene.hpp"

namespace slash {

class Slash_API SceneManager {
 public:
  static void Init();
  static void Destroy();

  static void PrepareScene();
  static void DrawScene(double dt);
  static void AddToScene(const Entity entity);
  static auto GetScene() -> Scene &;

 private:
  static inline Scene *scene_ = nullptr;
};

}

#endif // SLASHENGINE_SRC_SCENE_SCENE_MANAGER_HPP_
