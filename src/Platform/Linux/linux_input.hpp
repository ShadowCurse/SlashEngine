#ifndef SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_
#define SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_

#include "Core/input.hpp"

namespace slash {

class LinuxInput final : public Input {
protected:
  [[nodiscard]] bool IsKeyPressedImpl(int keycode) final;
  [[nodiscard]] bool IsMouseButtonPressedImpl(int button) final;
  [[nodiscard]] std::pair<float, float> GetMousePositionImpl() final;
  [[nodiscard]] float GetMouseXImpl() final;
  [[nodiscard]] float GetMouseYImpl() final;
};

} // namespace slash

#endif // SLASHENGINE_PLATFORM_LINUX_LINUX_INPUT_H_