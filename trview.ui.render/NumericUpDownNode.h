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
            class NumericUpDownNode final : public RenderNode
            {
            public:
                explicit NumericUpDownNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source,
                    NumericUpDown* numeric_up_down, const graphics::Texture& up, const graphics::Texture& down);
                virtual ~NumericUpDownNode() = default;
            protected:
                virtual void render_self(graphics::ISprite& sprite) override;
            private:
                NumericUpDown* _numeric_up_down{ nullptr };
            };
        }
    }
}