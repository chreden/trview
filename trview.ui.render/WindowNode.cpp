#include "WindowNode.h"
#include <trview.ui/Window.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            WindowNode::WindowNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Window* window)
                : _window(window), RenderNode(device, render_target_source, window)
            {
            }

            WindowNode::~WindowNode()
            {
            }

            void WindowNode::render_self(graphics::ISprite&)
            {
                _render_target->clear(_window->background_colour());
            }
        }
    }
}