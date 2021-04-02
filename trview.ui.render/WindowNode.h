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
                WindowNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Window* window);
                virtual ~WindowNode();
            protected:
                virtual void render_self(graphics::ISprite& sprite) override;
            private:
                Window* _window;
            };
        }
    }
}