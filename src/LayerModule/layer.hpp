#ifndef SLASHENGINE_CORE_LAYER_H_
#define SLASHENGINE_CORE_LAYER_H_

#include "Events/event.hpp"
#include "Core/core.hpp"
#include "Core/time_step.hpp"

namespace slash {

class Slash_API Layer {
 protected:
  Layer() = default;

 public:
  virtual ~Layer() = default;
  virtual void on_attach() = 0;
  virtual void on_detach() = 0;
  virtual void on_update(TimeStep ts) = 0;
  virtual void on_event(Event &event) = 0;
};

} // namespace slash

#endif // SLASHENGINE_CORE_LAYER_H_