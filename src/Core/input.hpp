#ifndef SLASHENGINE_CORE_INPUT_H
#define SLASHENGINE_CORE_INPUT_H

#include "core.hpp"

namespace slash {

class Slash_API Input {
public:
  Input(const Input &) = delete;
  virtual ~Input() = default;
  
  Input &operator=(const Input &) = delete;

  inline static bool IsKeyPressed(int keycode) {
    return p_instance_->IsKeyPressedImpl(keycode);
  }
  inline static bool IsMouseButtonPressed(int button) {
    return p_instance_->IsMouseButtonPressedImpl(button);
  }
  inline static std::pair<float, float> GetMousePosition() {
    return p_instance_->GetMousePositionImpl();
  }
  inline static float GetMouseX() { return p_instance_->GetMouseXImpl(); }
  inline static float GetMouseY() { return p_instance_->GetMouseYImpl(); }

protected:
  Input() = default;

  virtual bool IsKeyPressedImpl(int keycode) = 0;
  virtual bool IsMouseButtonPressedImpl(int button) = 0;
  virtual std::pair<float, float> GetMousePositionImpl() = 0;
  virtual float GetMouseXImpl() = 0;
  virtual float GetMouseYImpl() = 0;

private:
  static std::unique_ptr<Input> p_instance_;
};
} // namespace slash
#endif // SLASHENGINE_CORE_INPUT_H