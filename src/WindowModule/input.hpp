#ifndef SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_
#define SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_

#include "Core/core.hpp"

namespace slash {

class Slash_API Input {
 public:
  [[nodiscard]] bool IsKeyPressed(int keycode);
  [[nodiscard]] bool IsMouseButtonPressed(int button);
  [[nodiscard]] auto GetMousePosition() -> std::pair<float, float>;
  [[nodiscard]] auto GetMouseX() -> float;
  [[nodiscard]] auto GetMouseY() -> float;
};

} // namespace slash

#endif // SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_