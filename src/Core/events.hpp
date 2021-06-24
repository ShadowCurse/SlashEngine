#ifndef SLASHENGINE_SRC_CORE_EVENTS_HPP_
#define SLASHENGINE_SRC_CORE_EVENTS_HPP_

#include "Core/core.hpp"
#include "Ecs/entity.hpp"

namespace slash {

struct AppClose {};

// TODO move this to render module
struct CreateRenderable {
  Entity entity;
};

}

#endif //SLASHENGINE_SRC_CORE_EVENTS_HPP_
