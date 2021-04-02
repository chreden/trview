#include "Renderer.h"

#include <trview.ui/Window.h>
#include <trview.ui/Label.h>
#include <trview.ui/Image.h>
#include <trview.ui/Button.h>

#include "WindowNode.h"
#include "LabelNode.h"
#include "ImageNode.h"
#include "ButtonNode.h"

namespace trview
{
    namespace ui
    {
        namespace render
        {
            Renderer::Renderer(const std::shared_ptr<graphics::IDevice>& device,
                const graphics::IRenderTarget::SizeSource& render_target_source,
                const graphics::IFontFactory& font_factory,
                const Size& host_size,
                const graphics::ISprite::Source& sprite_source)
                : _device(device), 
                _font_factory(font_factory),
                _render_target_source(render_target_source),
                _sprite(sprite_source(host_size)),
                _host_size(host_size)
            {
                D3D11_DEPTH_STENCIL_DESC ui_depth_stencil_desc;
                memset(&ui_depth_stencil_desc, 0, sizeof(ui_depth_stencil_desc));
                _depth_stencil_state = device->create_depth_stencil_state(ui_depth_stencil_desc);
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

                // Attempt to find the correct type for each control in the hierarchy.
                // Create a duplicate hierarchy in the renderer.
                _root_node = process_control(control);
            }

            std::unique_ptr<RenderNode> Renderer::process_control(Control* control)
            {
                std::unique_ptr<RenderNode> node;

                // TODO: Use DI
                // Figure out what type of control this is and create the appropriate
                // rendering node.
                if (auto label = dynamic_cast<Label*>(control))
                {
                    node = std::make_unique<LabelNode>(_device, _render_target_source, label, _font_factory);
                }
                else if (auto button = dynamic_cast<Button*>(control))
                {
                    node = std::make_unique<ButtonNode>(_device, _render_target_source, button);
                }
                else if (auto image = dynamic_cast<Image*>(control))
                {
                    node = std::make_unique<ImageNode>(_device, _render_target_source, image);
                }
                else if (auto window = dynamic_cast<Window*>(control))
                {
                    node = std::make_unique<WindowNode>(_device, _render_target_source, window);
                }

                // Process the child nodes and build the structure to match the UI model.
                auto children = control->child_elements(true);
                for (auto child : children)
                {
                    node->add_child(process_control(child));
                }

                auto* node_ptr = node.get();
                node_ptr->_token_store += control->on_hierarchy_changed += [this, control, node_ptr]()
                {
                    node_ptr->set_hierarchy_changed(true);
                    _hierarchy_changed = true;
                };

                return node;
            }

            void Renderer::update_hierarchy(RenderNode& node)
            {
                if (node.heirarchy_changed())
                {
                    node.clear_children();
                    auto children = node.control()->child_elements(true);
                    for (auto child : children)
                    {
                        node.add_child(process_control(child));
                    }
                    node.set_hierarchy_changed(false);
                }
                else
                {
                    for (auto& child : node._child_nodes)
                    {
                        update_hierarchy(*child);
                    }
                }
            }

            void Renderer::render()
            {
                auto context = _device->context();
                context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);

                // The job of the renderer is to render the texture that has been generated
                // by the top level control. This texture is generated from the child windows
                // drawing in turn.
                if (_root_node && _root_node->visible())
                {
                    // Check for any changes in the hierarchy and regenerate if required.
                    if (_hierarchy_changed)
                    {
                        update_hierarchy(*_root_node);
                        _hierarchy_changed = false;
                    }

                    {
                        graphics::RenderTargetStore render_target_store(context);

                        // The nodes will detect whether or not they need to actually re-render
                        // their content, based on changes to the control that they are watching.
                        _root_node->render(*_sprite); 
                    }

                    auto texture = _root_node->node_texture();
                    if (texture.can_use_as_resource())
                    {
                        _sprite->render(texture, 0, 0, _host_size.width, _host_size.height);
                    }
                }
            }

            // Set the size of the host render area.
            // size: The size of the render area
            void Renderer::set_host_size(const Size& size)
            {
                _host_size = size;
                _sprite->set_host_size(size);
            }
        }
    }
}