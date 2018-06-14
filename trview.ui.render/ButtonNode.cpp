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
            ButtonNode::ButtonNode(const ComPtr<ID3D11Device>& device, Button* button)
                : RenderNode(device, button), _button(button), _blank(_device, 1, 1, std::vector<uint32_t>(1, 0xffffffff))
            {
            }

            ButtonNode::~ButtonNode()
            {
            }

            void ButtonNode::render_self(const ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite)
            {
                _render_target->clear(context, Color(0.0f, 0.0f, 0.0f));
                graphics::RenderTargetStore rt_store(context);
                graphics::ViewportStore vp_store(context);
                graphics::SpriteSizeStore s_store(sprite, _render_target->width(), _render_target->height());
                _render_target->apply(context);
                sprite.render(context, _blank, 1, 1, _button->size().width - 2, _button->size().height - 2,
                    Color(0.4f, 0.4f, 0.4f));
            }
        }
    }
}
