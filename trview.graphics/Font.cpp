#include "Font.h"
#include "Texture.h"
#include <trview.common/Size.h>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace trview
{
    namespace graphics
    {
        Font::Font(const std::shared_ptr<SpriteFont>& font, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment)
            : _font(font), _text_alignment(text_alignment), _paragraph_alignment(paragraph_alignment)
        {
        }

        void Font::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float width, float height)
        {
            if (!_batch)
            {
                _batch = std::make_unique<SpriteBatch>(context.Get());
            }

            // Make sure the sprite batch uses our blend state instead of setting its own.
            ComPtr<ID3D11BlendState> blend_state;
            context->OMGetBlendState(&blend_state, nullptr, nullptr);

            // Calculate the position at which to render the text based on the alignment settings.
            const auto size = measure(text);
            float x = 0;
            float y = 0;

            if (_text_alignment == TextAlignment::Centre)
            {
                x = width * 0.5f - size.width * 0.5f;
            }

            if (_paragraph_alignment == ParagraphAlignment::Centre)
            {
                y = height * 0.5f - size.height * 0.5f;
            }

            _batch->Begin(SpriteSortMode_Deferred, blend_state.Get());
            _font->DrawString(_batch.get(), text.c_str(), XMVectorSet(round(x), round(y), 0, 0), XMVectorSet(1, 1, 1, 1), 0, XMVectorZero(), XMVectorSet(1, 1, 1, 1));
            _batch->End();
        }

        // Determines the size in pixels that the text specified will be when rendered.
        // text: The text to measure.
        // Returns: The size in pixels required.
        Size Font::measure(const std::wstring& text) const
        {
            XMFLOAT2 size;
            XMStoreFloat2(&size, _font->MeasureString(text.c_str()));
            return Size(size.x, size.y);
        }
    }
}
