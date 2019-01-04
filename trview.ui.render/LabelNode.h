#pragma once

#include "WindowNode.h"
#include <trview.ui/IFontMeasurer.h>

namespace trview
{
    namespace graphics
    {
        class Font;
        class FontFactory;
    }

    namespace ui
    {
        class Label;

        namespace render
        {
            class LabelNode : public WindowNode, public IFontMeasurer
            {
            public:
                LabelNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Label* label, const graphics::FontFactory& font_factory);
                virtual ~LabelNode();
                virtual Size measure(const std::wstring& text) const override;
            protected:
                virtual void render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite) override;
            private:
                // Generate the font texture and other textures required to render the label. This will also
                // resize the label if the label has been set to auto size mode.
                void generate_font_texture();

                Label*                          _label;
                std::unique_ptr<graphics::Font> _font;
            };
        }
    }
}