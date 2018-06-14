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
                explicit ButtonNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Button* button);

                /// Destructor for the button node.
                virtual ~ButtonNode();
            protected:
                /// Render the node.
                /// @param context The D3D context to use to render.
                /// @param sprite The sprite to use to render.
                virtual void render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite) override;
            private:
                graphics::Texture _blank;
                Button*           _button;
            };
        }
    }
}

