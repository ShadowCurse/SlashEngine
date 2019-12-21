#ifndef SL_INPUT
#define SL_INPUT

#include "Core/Core.hpp"

namespace Slash
{
    class Slash_API Input
    {

    protected:
        Input() = default;
    
    public:
        Input(const Input&) = delete;
        Input& operator=(const Input&) = delete;

        inline static bool IsKeyPressed(int keycode) { return _instance->IsKeyPressedImpl(keycode); }
        inline static bool IsMouceButtonPressed(int button) { return _instance->IsMouseButtonPressedImpl(button); }
        inline static std::pair<float, float> GetMoucePosition() { return _instance->GetMoucePositionImpl(); }
        inline static float GetMouseX() { return _instance->GetMouseXImpl(); }
        inline static float GetMouseY() { return _instance->GetMouseYImpl(); }

    protected:

        virtual bool IsKeyPressedImpl(int keycode) = 0;
        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual std::pair<float, float> GetMoucePositionImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

    private:
        static Unique<Input> _instance;
    };
} // namespace Slash


#endif