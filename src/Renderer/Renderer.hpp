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
        template<typename API>
        static void Create() { m_apiInstance = CreateUnique<API>(); }
        template<typename API>
        static API& GetAPI() { return *static_cast<API*>(m_apiInstance.get()); }
        static void AddWindow(std::shared_ptr<Window> window);
        static void Init();
        static void Destroy();
        static void DrawFrame(float time);

    private:
        inline static Unique<RendererAPI> m_apiInstance = nullptr;
    };

} // namespace Slash

#endif