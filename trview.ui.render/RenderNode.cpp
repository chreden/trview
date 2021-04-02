#include "RenderNode.h"
#include <trview.graphics/RenderTarget.h>

#include <trview.ui/Control.h>

using namespace Microsoft::WRL;
using namespace trview::graphics;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            RenderNode::RenderNode(const std::shared_ptr<IDevice>& device, const IRenderTarget::SizeSource& render_target_source, Control* control)
                : _device(device), _render_target_source(render_target_source), _control(control)
            {
                regenerate_texture();
                _token_store += _control->on_size_changed += [&](auto) {regenerate_texture(); };
                _token_store += _control->on_invalidate += [&]() { _needs_redraw = true; };
            }

            RenderNode::~RenderNode()
            {
            }

            const Texture& RenderNode::node_texture() const
            {
                return _render_target->texture();
            }

            void RenderNode::render(ISprite& sprite)
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
                    child->render(sprite);
                }

                auto context = _device->context();
                RenderTargetStore render_target_store(context);
                ViewportStore vp_store(context);
                SpriteSizeStore s_store(sprite, _render_target->size());
                _render_target->apply();

                render_self(sprite);

                std::vector<RenderNode*> children;
                std::transform(_child_nodes.begin(), _child_nodes.end(), std::back_inserter(children),
                    [](auto& child) { return child.get(); });
                std::sort(children.begin(), children.end(), [](const auto& l, const auto& r) { return l->z() > r->z(); });

                for (auto& child : children)
                {
                    if (!child->visible())
                    {
                        continue;
                    }

                    // Render the child in the correct position on the render target.
                    auto pos = child->position();
                    auto size = child->size();
                    sprite.render(child->node_texture(), pos.x, pos.y, size.width, size.height);
                }

                _needs_redraw = false;
            }

            void RenderNode::add_child(std::unique_ptr<RenderNode>&& child)
            {
                _child_nodes.push_back(std::move(child));
                _needs_redraw = true;
            }

            Control* RenderNode::control() const
            {
                return _control;
            }

            void RenderNode::clear_children()
            {
                _child_nodes.clear();
                _needs_redraw = true;
            }

            bool RenderNode::heirarchy_changed() const
            {
                return _hierarchy_changed;
            }

            Point RenderNode::position() const
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
                auto size = _control->size();
                size = Size(size.width == 0 ? 1 : size.width, size.height == 0 ? 1 : size.height);
                _render_target = _render_target_source(static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height), IRenderTarget::DepthStencilMode::Disabled);
            }

            void RenderNode::set_hierarchy_changed(bool value)
            {
                _hierarchy_changed = value;
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

            int RenderNode::z() const
            {
                return _control->z();
            }
        }
    }
}