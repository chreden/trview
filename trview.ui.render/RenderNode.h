#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <cstdint>

#include <memory>
#include <vector>

#include <trview.ui/Point.h>
#include <trview.ui/Size.h>

namespace trview
{
    namespace ui
    {
        class Control;

        namespace render
        {
            class Sprite;

            // Render node controls all the basic functionality of the rendering system
            // for the UI, such as the render targets that are required and it will help
            // to manage the compositing of the final image to be rendered to the screen.
            class RenderNode
            {
            public:
                RenderNode(CComPtr<ID3D11Device> device, Control* control);

                virtual ~RenderNode() = 0;

                CComPtr<ID3D11ShaderResourceView> node_texture_view() const;

                void render(CComPtr<ID3D11DeviceContext> context, Sprite& sprite);

                void add_child(std::unique_ptr<RenderNode>&& child);

                ui::Point position() const;

                ui::Size size() const;

                bool visible() const;
            protected:
                virtual void render_self(CComPtr<ID3D11DeviceContext> context, Sprite& sprite) = 0;

                void regenerate_texture();

                CComPtr<ID3D11Device>                    _device;
                CComPtr<ID3D11Texture2D>                 _node_texture;
                CComPtr<ID3D11ShaderResourceView>        _node_texture_view;
                CComPtr<ID3D11RenderTargetView>          _render_target_view;
                std::vector<std::unique_ptr<RenderNode>> _child_nodes;
                Control*                                 _control;
            };
        }
    }
}