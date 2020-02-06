#include "Renderer/Renderer.hpp"

namespace Slash
{
    void Renderer::AddWindow(std::shared_ptr<Window> window)
    {
        m_apiInstance->AddWindow(window);
    }
    void Renderer::Init()
    {
        m_apiInstance->Init();
    }

    void Renderer::Destroy()
    {
        m_apiInstance->Destroy();
    }

    void Renderer::DrawFrame(float time)
    {
        m_apiInstance->DrawFrame(time);
    }
} // namespace Slash
