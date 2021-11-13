#include "NumericUpDownNode.h"
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Button.h>

using namespace trview::graphics;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            NumericUpDownNode::NumericUpDownNode(const std::shared_ptr<IDevice>& device, const IRenderTarget::SizeSource& render_target_source, NumericUpDown* numeric_up_down)
                : RenderNode(device, render_target_source, numeric_up_down), _numeric_up_down(numeric_up_down)
            {
                auto up = numeric_up_down->find<Button>("numeric_up_down_up");
                std::vector<uint32_t> up_pixels(1, 0xffffffff);
                const auto up_image = Texture(*device, 1u, 1u, up_pixels);
                up->set_images(up_image, up_image);

                auto down = numeric_up_down->find<Button>("numeric_up_down_down");
                std::vector<uint32_t> down_pixels(1, 0xff0080ff);
                const auto down_image = Texture(*device, 1u, 1u, down_pixels);
                down->set_images(down_image, down_image);
            }

            void NumericUpDownNode::render_self(ISprite& sprite)
            {
                _render_target->clear(_numeric_up_down->background_colour());
            }
        }
    }
}
