#include "ButtonNode.h"
#include <trview.ui/Button.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            ButtonNode::ButtonNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Button* button)
                : RenderNode(device, render_target_source, button), _button(button), _blank(*device, 1, 1, std::vector<uint32_t>(1, 0xffffffff))
            {
            }

            ButtonNode::~ButtonNode()
            {
            }

            void ButtonNode::render_self(graphics::ISprite& sprite)
            {
                const float thickness = _button->border_thickness();
                if (thickness)
                {
                    _render_target->clear(Colour::Black);
                }

                auto context = _device->context();
                graphics::RenderTargetStore rt_store(context);
                graphics::ViewportStore vp_store(context);
                graphics::SpriteSizeStore s_store(sprite, _render_target->size());
                _render_target->apply();

                sprite.render(_blank, thickness, thickness, _button->size().width - 2.0f * thickness, _button->size().height - 2.0f * thickness,
                    _button->background_colour());
            }
        }
    }
}
