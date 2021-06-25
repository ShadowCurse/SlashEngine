#ifndef SLASHENGINE_SRC_WINDOWMODULE_EVENTS_HPP_
#define SLASHENGINE_SRC_WINDOWMODULE_EVENTS_HPP_

namespace slash {

struct MouseMovedEvent {
  double x{};
  double y{};
};

struct MouseScrolledEvent {
  double x_offset{};
  double y_offset{};
};

struct MouseButtonPressedEvent {
  int button{};
};

struct MouseButtonReleasedEvent {
  int button{};
};

struct KeyPressedEvent {
  int key_code{};
  int repeat_count{};
};

struct KeyReleasedEvent {
  int key_code{};
};

struct KeyTypedEvent {
  int key_code{};
};

}

#endif //SLASHENGINE_SRC_WINDOWMODULE_EVENTS_HPP_
