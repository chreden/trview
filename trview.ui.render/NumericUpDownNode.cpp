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
            NumericUpDownNode::NumericUpDownNode(const std::shared_ptr<IDevice>& device, const IRenderTarget::SizeSource& render_target_source,
                NumericUpDown* numeric_up_down, const graphics::Texture& up, const graphics::Texture& down)
                : RenderNode(device, render_target_source, numeric_up_down), _numeric_up_down(numeric_up_down)
            {
                numeric_up_down->find<Button>(NumericUpDown::Names::up)->set_images(up, up);
                numeric_up_down->find<Button>(NumericUpDown::Names::down)->set_images(down, down);
            }

            void NumericUpDownNode::render_self(ISprite& sprite)
            {
                _render_target->clear(_numeric_up_down->background_colour());
            }
        }
    }
}
