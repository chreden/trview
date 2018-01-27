#pragma once

#include <atlbase.h>
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
                WindowNode(CComPtr<ID3D11Device> device, Window* window);
                virtual ~WindowNode();
            protected:
                virtual void render_self(CComPtr<ID3D11DeviceContext> context, Sprite& sprite) override;
            private:
                Window* _window;
            };
        }
    }
}