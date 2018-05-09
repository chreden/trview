#include "RenderNode.h"
#include <vector>
#include <algorithm>

#include <trview.ui/Control.h>
#include "Sprite.h"
#include "RenderTargetStore.h"

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

            ComPtr<ID3D11ShaderResourceView> RenderNode::node_texture_view() const
            {
                return _node_texture_view;
            }

            void RenderNode::render(const ComPtr<ID3D11DeviceContext>& context, Sprite& sprite)
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

                RenderTargetStore render_target_store(context);
                render_self(context, sprite);
                context->OMSetRenderTargets(1, _render_target_view.GetAddressOf(), nullptr);
                for (auto& child : _child_nodes)
                {
                    if (!child->visible())
                    {
                        continue;
                    }

                    // Render the child in the correct position on the render target.
                    auto pos = child->position();
                    auto size = child->size();
                    sprite.render(context, child->node_texture_view(), pos.x, pos.y, size.width, size.height);
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

            ui::Size RenderNode::size() const
            {
                return _control->size();
            }

            bool RenderNode::visible() const
            {
                return _control->visible();
            }

            void RenderNode::regenerate_texture()
            {
                // Reset existing textures.
                _node_texture = nullptr;
                _node_texture_view = nullptr;
                _render_target_view = nullptr;

                auto size = _control->size();
                uint32_t width = static_cast<uint32_t>(size.width);
                uint32_t height = static_cast<uint32_t>(size.height);

                std::vector<uint32_t> pixels(width * height, 0x00000000);

                D3D11_SUBRESOURCE_DATA srd;
                memset(&srd, 0, sizeof(srd));
                srd.pSysMem = &pixels[0];
                srd.SysMemPitch = sizeof(uint32_t) * width;

                D3D11_TEXTURE2D_DESC desc;
                memset(&desc, 0, sizeof(desc));
                desc.Width = width;
                desc.Height = height;
                desc.MipLevels = desc.ArraySize = 1;
                desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                desc.SampleDesc.Count = 1;
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;

                _device->CreateTexture2D(&desc, &srd, &_node_texture);
                _device->CreateShaderResourceView(_node_texture.Get(), nullptr, &_node_texture_view);
                _device->CreateRenderTargetView(_node_texture.Get(), nullptr, &_render_target_view);
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