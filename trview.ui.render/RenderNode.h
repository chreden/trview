#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>

#include <memory>
#include <vector>

#include <trview.common/TokenStore.h>
#include <trview.common/Point.h>
#include <trview.graphics/IRenderTarget.h>
#include <trview.graphics/IDevice.h>

namespace trview
{
    struct Size;

    namespace graphics
    {
        struct ISprite;
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
                friend class Renderer;

                RenderNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Control* control);

                virtual ~RenderNode() = 0;

                const graphics::Texture& node_texture() const;

                void render(graphics::ISprite& sprite);

                void add_child(std::unique_ptr<RenderNode>&& child);

                Control* control() const;

                void clear_children();

                bool heirarchy_changed() const;

                Point position() const;

                void set_hierarchy_changed(bool value);

                Size size() const;

                bool visible() const;

                int z() const;
            protected:
                virtual void render_self(graphics::ISprite& sprite) = 0;
                // Determines if the control itself needs to redraw.
                bool needs_redraw() const;
                // Determines if the control has any children that need to be re-rendered on to the
                // render target for the control (they have been redrawn).
                bool needs_recompositing() const;
                void regenerate_texture();

                TokenStore _token_store;
                std::shared_ptr<graphics::IDevice>       _device;
                std::unique_ptr<graphics::IRenderTarget> _render_target;
                graphics::IRenderTarget::SizeSource      _render_target_source;
                std::vector<std::unique_ptr<RenderNode>> _child_nodes;
                Control*                                 _control;
                bool                                     _needs_redraw{ true };
                bool                                     _hierarchy_changed{ false };
            };
        }
    }
}