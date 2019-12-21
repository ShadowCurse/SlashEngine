#ifndef SL_RENDERERAPI
#define SL_RENDERERAPI

namespace Slash
{

    class RendererAPI
    {
    protected:
        RendererAPI() {};
    public:
        virtual ~RendererAPI() = default;
        virtual void Init() = 0;
        virtual void Destroy() = 0;
    };

} // namespace Slash

#endif