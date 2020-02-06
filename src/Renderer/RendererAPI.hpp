#ifndef SL_RENDERERAPI
#define SL_RENDERERAPI

#include "SlashPCH.hpp"
#include "Core/Window.hpp"

namespace Slash
{
    class RendererAPI
    {
    protected:
        RendererAPI() = default;
    public:
        virtual ~RendererAPI() = default;
        virtual void AddWindow(std::shared_ptr<Window> window) = 0;
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void DrawFrame(float time) = 0;
    };

} // namespace Slash

#endif