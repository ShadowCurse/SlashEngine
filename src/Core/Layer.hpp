#ifndef SL_LAYER
#define SL_LAYER

#include "Core/Core.hpp"
#include "Core/Timestep.hpp"
#include "Events/Event.hpp"

namespace Slash
{

    class Slash_API Layer
    {
    protected:
        Layer() = default;
    public:
        virtual ~Layer() = default;
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(Timestep ts) = 0;
        virtual void OnEvent(Event& event) = 0;
    };

} // namespace Slash

#endif