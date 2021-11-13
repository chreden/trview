#pragma once

#include <trview.graphics/IRenderTarget.h>
#include "RenderNode.h"

namespace trview
{
    namespace ui
    {
        class NumericUpDown;

        namespace render
        {
            /// Rendering node that can render a button.
            class NumericUpDownNode final : public RenderNode
            {
            public:
                explicit NumericUpDownNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, NumericUpDown* numeric_up_down);
                virtual ~NumericUpDownNode() = default;
            protected:
                virtual void render_self(graphics::ISprite& sprite) override;
            private:
                NumericUpDown* _numeric_up_down{ nullptr };
            };
        }
    }
}
