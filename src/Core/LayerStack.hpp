#ifndef SL_LAYERSTACK
#define SL_LAYERSTACK

#include "Core/Core.hpp"
#include "Layer.hpp"
#include "slash_pch.hpp"

namespace Slash
{

    class Slash_API LayerStack
    {
    public:
        LayerStack();
        virtual ~LayerStack();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* layer);

        std::vector<Layer*>::iterator begin() { return _layers.begin(); }
        std::vector<Layer*>::iterator end() { return _layers.end(); }
    
    private:
        std::vector<Layer*> _layers;
        uint _layerInsertIndex = 0;    
    };

} // namespace Slash


#endif