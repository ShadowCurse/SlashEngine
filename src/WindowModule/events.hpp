#ifndef SLASHENGINE_SRC_WINDOWMODULE_EVENTS_HPP_
#define SLASHENGINE_SRC_WINDOWMODULE_EVENTS_HPP_

#include "Core/core.hpp"

namespace slash {

struct Slash_API MouseMovedEvent {
  double x{};
  double y{};
};

struct Slash_API MouseScrolledEvent {
  double x_offset{};
  double y_offset{};
};

struct Slash_API MouseButtonPressedEvent {
  int button{};
};

struct Slash_API MouseButtonReleasedEvent {
  int button{};
};

struct Slash_API KeyPressedEvent {
  int key_code{};
  int repeat_count{};
};

struct Slash_API KeyReleasedEvent {
  int key_code{};
};

struct Slash_API KeyTypedEvent {
  int key_code{};
};

}

#endif //SLASHENGINE_SRC_WINDOWMODULE_EVENTS_HPP_
