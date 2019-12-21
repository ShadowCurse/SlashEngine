#ifndef SL_LAYER
#define SL_LAYER

#include "Core/Core.hpp"
#include "Core/Timestep.hpp"
#include "Events/Event.hpp"

namespace Slash
{

    class Slash_API Layer
    {
    public:
        Layer() {}
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnEvent(Event& event) {}
    };

} // namespace Slash

#endif