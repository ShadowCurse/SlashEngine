#ifndef SL_RENDERER
#define SL_RENDERER

#include "Core/Window.hpp"
#include "Renderer/RendererAPI.hpp"

namespace Slash
{

    class Renderer
    {
    private:
        Renderer() {};
    public:
        virtual ~Renderer() = default;
        static void Init();
        static void Destroy();

    private:
        static Unique<RendererAPI> _apiInstance;
    };

} // namespace Slash

#endif