#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>

#include <memory>
#include <vector>

#include <trview.ui/Point.h>
#include <trview.graphics/RenderTarget.h>

namespace trview
{
    struct Size;

    namespace graphics
    {
        class Sprite;
    }

    namespace ui
    {
        class Control;

        namespace render
        {
            // Render node controls all the basic functionality of the rendering system
            // for the UI, such as the render targets that are required and it will help
            // to manage the compositing of the final image to be rendered to the screen.
            class RenderNode
            {
            public:
                RenderNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Control* control);

                virtual ~RenderNode() = 0;

                const graphics::Texture& node_texture() const;

                void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite);

                void add_child(std::unique_ptr<RenderNode>&& child);

                ui::Point position() const;

                Size size() const;

                bool visible() const;
            protected:
                virtual void render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite) = 0;

            public:
                // Determines if the control itself needs to redraw.
                bool needs_redraw() const;

                // Determines if the control has any children that need to be re-rendered on to the
                // render target for the control (they have been redrawn).
                bool needs_recompositing() const;

                void regenerate_texture();

                Microsoft::WRL::ComPtr<ID3D11Device>     _device;
                std::unique_ptr<graphics::RenderTarget>  _render_target;
                std::vector<std::unique_ptr<RenderNode>> _child_nodes;
                Control*                                 _control;
                bool                                     _needs_redraw{ true };
            };
        }
    }
}