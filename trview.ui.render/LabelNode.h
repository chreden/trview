#pragma once

#include "WindowNode.h"
#include "Font.h"
#include "FontTexture.h"

namespace trview
{
    namespace ui
    {
        class Label;

        namespace render
        {
            class FontFactory;

            class LabelNode : public WindowNode
            {
            public:
                LabelNode(CComPtr<ID3D11Device> device, Label* label, FontFactory& font_factory);
                virtual ~LabelNode();
            protected:
                virtual void render_self(CComPtr<ID3D11DeviceContext> context, Sprite& sprite) override;
            private:
                // Generate the font texture and other textures required to render the label. This will also
                // resize the label if the label has been set to auto size mode.
                void generate_font_texture();

                Label*                _label;
                std::unique_ptr<Font> _font;
                FontTexture           _font_texture;
            };
        }
    }
}