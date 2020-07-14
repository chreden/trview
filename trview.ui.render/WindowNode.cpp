#include "WindowNode.h"
#include <trview.ui/Window.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            WindowNode::WindowNode(const graphics::Device& device, Window* window)
                : _window(window), RenderNode(device, window)
            {
            }

            WindowNode::~WindowNode()
            {
            }

            void WindowNode::render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite&)
            {
                _render_target->clear(context, _window->background_colour());
            }
        }
    }
}