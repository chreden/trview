#pragma once

#include "RenderNode.h"

namespace trview
{
    namespace ui
    {
        class Button;

        namespace render
        {
            class ButtonNode : public RenderNode
            {
            public:
                explicit ButtonNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Button* button);
                virtual ~ButtonNode();
            protected:
                virtual void render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite) override;
            private:
                graphics::Texture _blank;
                Button*           _button;
            };
        }
    }
}

