#ifndef SLASHENGINE_SRC_SCENE_SCENE_HPP_
#define SLASHENGINE_SRC_SCENE_SCENE_HPP_

#include "Core/core.hpp"
#include "camera.hpp"
#include "Ecs/ecs.hpp"

namespace slash {

using Entity = uint32_t;

class Scene {
 public:
  Scene() = default;

  auto GetECS() -> ECS & { return ecs_; }
  auto GetCamera() -> Camera & { return camera_; }

 private:
  Camera camera_;
  ECS ecs_;
};

}

#endif // SLASHENGINE_SRC_SCENE_SCENE_HPP_
