#include "Renderer/Renderer.hpp"
#include "VulkanRenderer/VulkanRendererAPI.hpp"

namespace Slash
{
    
    Unique<RendererAPI> Renderer::_apiInstance = CreateUnique<VulkanRendererAPI>();

    void Renderer::Init()
    {
        _apiInstance->Init();
    }

    void Renderer::Destroy()
    {
        _apiInstance->Destroy();
    }

    void Renderer::DrawFrame()
    {
        _apiInstance->DrawFrame();
    }

} // namespace Slash
