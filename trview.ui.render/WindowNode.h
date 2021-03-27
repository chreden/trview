#pragma once

#include <wrl/client.h>
#include <d3d11.h>

#include "RenderNode.h"

namespace trview
{
    namespace ui
    {
        class Window;

        namespace render
        {
            class WindowNode : public RenderNode
            {
            public:
                WindowNode(const graphics::IDevice& device, Window* window);
                virtual ~WindowNode();
            protected:
                virtual void render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::ISprite& sprite) override;
            private:
                Window* _window;
            };
        }
    }
}