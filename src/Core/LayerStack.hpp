#ifndef SL_LAYERSTACK
#define SL_LAYERSTACK

#include "SlashPCH.hpp"
#include "Layer.hpp"
#include "Core/Core.hpp"

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