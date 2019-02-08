#include "ButtonNode.h"

#include <trview.graphics/Sprite.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>
#include <trview.graphics/SpriteSizeStore.h>
#include <trview.ui/Button.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            ButtonNode::ButtonNode(const graphics::Device& device, Button* button)
                : RenderNode(device, button), _button(button), _blank(_device, 1, 1, std::vector<uint32_t>(1, 0xffffffff))
            {
            }

            ButtonNode::~ButtonNode()
            {
            }

            void ButtonNode::render_self(const ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite)
            {
                const float thickness = _button->border_thickness();
                if (thickness)
                {
                    _render_target->clear(context, Color());
                }

                graphics::RenderTargetStore rt_store(context);
                graphics::ViewportStore vp_store(context);
                graphics::SpriteSizeStore s_store(sprite, _render_target->size());
                _render_target->apply(context);

                sprite.render(context, _blank, thickness, thickness, _button->size().width - 2.0f * thickness, _button->size().height - 2.0f * thickness,
                    Color(0.4f, 0.4f, 0.4f));
            }
        }
    }
}
