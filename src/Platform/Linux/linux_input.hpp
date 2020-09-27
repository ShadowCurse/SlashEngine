#ifndef SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_
#define SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_

#include "Core/input.hpp"

namespace slash {

class Slash_API LinuxInput final : public Input {
 protected:
  [[nodiscard]] bool IsKeyPressedImpl(int keycode) final;
  [[nodiscard]] bool IsMouseButtonPressedImpl(int button) final;
  [[nodiscard]] auto GetMousePositionImpl() -> std::pair<float, float> final;
  [[nodiscard]] auto GetMouseXImpl() -> float final;
  [[nodiscard]] auto GetMouseYImpl() -> float final;
};

} // namespace slash

#endif // SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_