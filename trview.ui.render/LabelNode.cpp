#include "LabelNode.h"

#include <trview.ui/Label.h>
#include <trview.graphics/FontFactory.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            LabelNode::LabelNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Label* label, graphics::FontFactory& font_factory)
                : WindowNode(device, label), 
                _font(font_factory.create_font(L"Arial", 
                                               label->text_size(), 
                                               label->text_alignment(),
                                               label->paragraph_alignment())), _label(label)
            {
                generate_font_texture();
                if (label->size_mode() == SizeMode::Auto)
                {
                    label->on_text_changed += [&](auto)
                    {
                        generate_font_texture();
                    };
                }
                
            }

            LabelNode::~LabelNode()
            {
            }

            void LabelNode::render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite)
            {
                WindowNode::render_self(context, sprite);
                const auto size = _label->size();
                _font->render(_font_texture, _label->text(), 0, 0, size.width, size.height);
            }

            // Generate the font texture and other textures required to render the label. This will also
            // resize the label if the label has been set to auto size mode.
            void LabelNode::generate_font_texture()
            {
                if (_label->size_mode() == SizeMode::Auto)
                {
                    auto new_size = _font->measure(_label->text());
                    _label->set_size(new_size);
                    regenerate_texture();
                }

                _font_texture = _font->create_texture(_render_target->texture(), _label->text_colour());
            }
        }
    }
}