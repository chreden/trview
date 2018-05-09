#include "WindowNode.h"
#include <trview.ui/Window.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            WindowNode::WindowNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Window* window)
                : _window(window), RenderNode(device, window)
            {
            }

            WindowNode::~WindowNode()
            {
            }

            void WindowNode::render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, Sprite&)
            {
                // Render this node.
                auto bg_colour = _window->background_colour();
                float colour[] = { bg_colour.r, bg_colour.g, bg_colour.b, bg_colour.a };
                context->ClearRenderTargetView(_render_target_view.Get(), colour);
            }
        }
    }
}