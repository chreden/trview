#include "RenderNode.h"
#include <vector>
#include <algorithm>

#include <trview.ui/Control.h>
#include <trview.graphics/Sprite.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>
#include <trview.graphics/SpriteSizeStore.h>
#include <trview.common/Size.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            RenderNode::RenderNode(const ComPtr<ID3D11Device>& device, Control* control)
                : _device(device), _control(control)
            {
                regenerate_texture();
                _control->on_size_changed += [&](auto) {regenerate_texture(); };
                _control->on_invalidate += [&]() { _needs_redraw = true; };
            }

            RenderNode::~RenderNode()
            {
            }

            const graphics::Texture& RenderNode::node_texture() const
            {
                return _render_target->texture();
            }

            void RenderNode::render(const ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite)
            {
                if (!needs_redraw() && !needs_recompositing())
                {
                    return;
                }

                if (!visible())
                {
                    _needs_redraw = false;
                    return;
                }

                for (auto& child : _child_nodes)
                {
                    child->render(context, sprite);
                }

                graphics::RenderTargetStore render_target_store(context);
                render_self(context, sprite);

                graphics::ViewportStore vp_store(context);
                graphics::SpriteSizeStore s_store(sprite, _render_target->width(), _render_target->height());
                _render_target->apply(context);

                for (auto& child : _child_nodes)
                {
                    if (!child->visible())
                    {
                        continue;
                    }

                    // Render the child in the correct position on the render target.
                    auto pos = child->position();
                    auto size = child->size();
                    sprite.render(context, child->node_texture(), pos.x, pos.y, size.width, size.height);
                }

                _needs_redraw = false;
            }

            void RenderNode::add_child(std::unique_ptr<RenderNode>&& child)
            {
                _child_nodes.push_back(std::move(child));
            }

            ui::Point RenderNode::position() const
            {
                return _control->position();
            }

            Size RenderNode::size() const
            {
                return _control->size();
            }

            bool RenderNode::visible() const
            {
                return _control->visible();
            }

            void RenderNode::regenerate_texture()
            {
                const auto size = _control->size();
                _render_target = std::make_unique<graphics::RenderTarget>(_device, static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height));
            }

            // Determines if the control itself needs to redraw.
            bool RenderNode::needs_redraw() const
            {
                return _needs_redraw;
            }

            // Determines if the control has any children that need to be re-rendered on to the
            // render target for the control (they have been redrawn).
            bool RenderNode::needs_recompositing() const
            {
                return std::any_of(_child_nodes.begin(), _child_nodes.end(),
                    [](const auto& n) { return n->needs_redraw() || n->needs_recompositing(); });
            }
        }
    }
}