#include "Renderer.h"

#include <trview.ui/Window.h>
#include <trview.ui/Label.h>
#include <trview.ui/Image.h>

#include "WindowNode.h"
#include "LabelNode.h"
#include "ImageNode.h"
#include <trview.graphics/Sprite.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/FontFactory.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            Renderer::Renderer(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, uint32_t host_width, uint32_t host_height)
                : _device(device), 
                _sprite(std::make_unique<graphics::Sprite>(device, shader_storage, host_width, host_height)),
                _font_factory(std::make_unique<FontFactory>()),
                _host_width(host_width), 
                _host_height(host_height)
            {
                D3D11_DEPTH_STENCIL_DESC ui_depth_stencil_desc;
                memset(&ui_depth_stencil_desc, 0, sizeof(ui_depth_stencil_desc));
                device->CreateDepthStencilState(&ui_depth_stencil_desc, &_depth_stencil_state);
            }

            Renderer::~Renderer()
            {
            }

            void Renderer::load(Control* control)
            {
                if (!control)
                {
                    return;
                }

                // Attempt to find the correct type for each control in the heirarchy.
                // Create a duplicate heirarchy in the renderer.
                _root_node = process_control(control);
            }

            std::unique_ptr<RenderNode> Renderer::process_control(Control* control)
            {
                std::unique_ptr<RenderNode> node;

                // Figure out what type of control this is and create the appropriate
                // rendering node.
                if (auto label = dynamic_cast<Label*>(control))
                {
                    node = std::make_unique<LabelNode>(_device, label, *_font_factory);
                }
                else if (auto image = dynamic_cast<Image*>(control))
                {
                    node = std::make_unique<ImageNode>(_device, image);
                }
                else if (auto window = dynamic_cast<Window*>(control))
                {
                    node = std::make_unique<WindowNode>(_device, window);
                }

                // Process the child nodes and build the structure to match the UI model.
                auto children = control->child_elements();
                for (auto child : children)
                {
                    node->add_child(process_control(child));
                }
                return node;
            }

            void Renderer::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
            {
                context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);

                // The job of the renderer is to render the texture that has been generated
                // by the top level control. This texture is generated from the child windows
                // drawing in turn.
                if (_root_node && _root_node->visible())
                {
                    {
                        graphics::RenderTargetStore render_target_store(context);

                        // The nodes will detect whether or not they need to actually re-render
                        // their content, based on changes to the control that they are watching.
                        _root_node->render(context, *_sprite); 
                    }

                    auto texture = _root_node->node_texture();
                    if (texture.can_use_as_resource())
                    {
                        _sprite->render(context, texture, 0, 0, static_cast<float>(_host_width), static_cast<float>(_host_height));
                    }
                }
            }

            // Set the size of the host render area.
            // width: The width of the render area.
            // height: The height of the render area.
            void Renderer::set_host_size(uint32_t width, uint32_t height)
            {
                _host_width = width;
                _host_height = height;
                _sprite->set_host_size(width, height);
            }
        }
    }
}