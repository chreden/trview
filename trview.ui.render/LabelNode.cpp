#include "LabelNode.h"

#include <trview.ui/Label.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            LabelNode::LabelNode(const graphics::IDevice& device, Label* label, const graphics::IFontFactory& font_factory)
                : WindowNode(device, label), 
                _font(font_factory.create_font("Arial", label->text_size(), label->text_alignment(), label->paragraph_alignment())), _label(label)
            {
                generate_font_texture();
                if (label->size_mode() == SizeMode::Auto)
                {
                    _token_store += label->on_text_changed += [&](auto)
                    {
                        generate_font_texture();
                    };
                }
                _label->set_measurer(this);
                _token_store += _label->on_deleting += [&]()
                {
                    _label = nullptr;
                };
            }

            LabelNode::~LabelNode()
            {
                if (_label)
                {
                    _label->set_measurer(nullptr);
                }
            }

            Size LabelNode::measure(const std::wstring& text) const
            {
                return _font->measure(text);
            }

            bool LabelNode::is_valid_character(wchar_t character) const
            {
                return _font->is_valid_character(character);
            }

            void LabelNode::render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite)
            {
                WindowNode::render_self(context, sprite);
                if (!_label)
                {
                    return;
                }

                const auto size = _label->size();
                _font->render(context, _label->text(), size.width, size.height, _label->text_colour());
            }

            // Generate the font texture and other textures required to render the label. This will also
            // resize the label if the label has been set to auto size mode.
            void LabelNode::generate_font_texture()
            {
                if (!_label)
                {
                    return;
                }

                if (_label->size_mode() == SizeMode::Auto)
                {
                    auto new_size = _font->measure(_label->text());
                    if (new_size != _label->size())
                    {
                        _label->set_size(new_size);
                        regenerate_texture();
                    }
                }
            }
        }
    }
}