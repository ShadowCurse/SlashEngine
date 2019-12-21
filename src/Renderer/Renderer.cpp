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

} // namespace Slash
