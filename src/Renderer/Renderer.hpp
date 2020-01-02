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
        static void DrawFrame();
        // static Instance that returns pouinter to actuial renderer

    private:
        static Unique<RendererAPI> _apiInstance;
        // static Uniqie<Renderer> _instance;
    };

} // namespace Slash

#endif