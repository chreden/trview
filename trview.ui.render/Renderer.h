#pragma once

#include <wrl/client.h>
#include <d3d11.h>

#include <memory>

#include "RenderNode.h"

#include <trview.ui/Control.h>

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        class Sprite;
    }

    namespace ui
    {
        namespace render
        {
            class FontFactory;

            class Renderer
            {
            public:
                explicit Renderer(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, uint32_t host_width, uint32_t host_height);

                ~Renderer();

                // Examine a control heirarchy and create the appropriate structures
                // required to render it. This will replace any existing rendering
                // structures.
                void load(Control* control);

                void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

                // Set the size of the host render area.
                // width: The width of the render area.
                // height: The height of the render area.
                void set_host_size(uint32_t width, uint32_t height);
            private:
                std::unique_ptr<RenderNode> process_control(Control* control);

                std::unique_ptr<RenderNode>                     _root_node;
                std::unique_ptr<graphics::Sprite>               _sprite;
                std::unique_ptr<FontFactory>                    _font_factory;
                Microsoft::WRL::ComPtr<ID3D11Device>            _device;
                Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
                uint32_t                                        _host_width;
                uint32_t                                        _host_height;
                
            };
        }
    }
}