#ifndef SL_LINUXINPUT
#define SL_LINUXINPUT

#include "Core/Input.hpp"

namespace Slash
{

    class LinuxInput : public Input
    {
    protected:
        virtual bool IsKeyPressedImpl(int keycode) override;
        
        virtual bool IsMouseButtonPressedImpl(int button) override;
        virtual std::pair<float, float> GetMoucePositionImpl() override;
        virtual float GetMouseXImpl() override;
        virtual float GetMouseYImpl() override;

    };

} // namespace Slash

#endif