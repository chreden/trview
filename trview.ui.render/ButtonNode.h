/// @file ButtonNode.h
/// @brief Rendering node that can render a button.
/// 
/// Does special operations for rendering button.

#pragma once

#include "RenderNode.h"

namespace trview
{
    namespace ui
    {
        class Button;

        namespace render
        {
            /// Rendering node that can render a button.
            class ButtonNode : public RenderNode
            {
            public:
                /// Create a new ButtonNode.
                /// @param device The device to use to render.
                /// @param button The button to render.
                explicit ButtonNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Button* button);

                /// Destructor for the button node.
                virtual ~ButtonNode();
            protected:
                /// Render the node.
                /// @param sprite The sprite to use to render.
                virtual void render_self(graphics::ISprite& sprite) override;
            private:
                graphics::Texture _blank;
                Button*           _button;
            };
        }
    }
}

