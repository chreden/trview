#pragma once

#include <wrl/client.h>
#include <d3d11.h>

#include <memory>

#include "RenderNode.h"

#include <trview.ui/Control.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        class FontFactory;
        class Sprite;
    }

    namespace ui
    {
        namespace render
        {
            class Renderer
            {
            public:
                explicit Renderer(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Size& host_size);

                ~Renderer();

                // Examine a control hierarchy and create the appropriate structures
                // required to render it. This will replace any existing rendering
                // structures.
                void load(Control* control);

                void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

                // Set the size of the host render area.
                // width: The size of the render area.
                void set_host_size(const Size& size);
            private:
                std::unique_ptr<RenderNode> process_control(Control* control);

                /// Go through the hierarchy and regenerate any nodes that need to be updated.
                /// @param node The node to process.
                void update_hierarchy(RenderNode& node);

                std::unique_ptr<RenderNode>                     _root_node;
                std::unique_ptr<graphics::Sprite>               _sprite;
                const graphics::Device&                         _device;
                Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
                const graphics::FontFactory&                    _font_factory;
                Size                                            _host_size;
                TokenStore                                      _token_store;
                bool                                            _hierarchy_changed{ false };
            };
        }
    }
}