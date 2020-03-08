#ifndef SLASHENGINE_CORE_LAYER_H_
#define SLASHENGINE_CORE_LAYER_H_

#include "Events/event.hpp"
#include "core.hpp"
#include "time_step.hpp"

namespace slash {

class Slash_API Layer {
protected:
  Layer() = default;

public:
  virtual ~Layer() = default;
  virtual void OnAttach() = 0;
  virtual void OnDetach() = 0;
  virtual void OnUpdate(TimeStep ts) = 0;
  virtual void OnEvent(Event &event) = 0;
};

} // namespace slash

#endif // SLASHENGINE_CORE_LAYER_H_