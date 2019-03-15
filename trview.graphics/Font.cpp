#include "Font.h"
#include "Texture.h"
#include <trview.common/Size.h>
#include <trview.common/Colour.h>
#include <algorithm>
#include <iterator>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace trview
{
    namespace graphics
    {
        namespace
        {
            /// Convert any characters that aren't in the sprite sheet to a safe character.
            /// @param text The text to convert.
            /// @result The santised string.
            std::wstring sanitise(const SpriteFont& font, const std::wstring& text)
            {
                std::wstring converted;
                std::transform(text.begin(), text.end(), std::back_inserter(converted),
                    [&](auto character)
                {
                    return font.ContainsCharacter(character) ? character : L'?';
                });
                return converted;
            }
        }


        Font::Font(const std::shared_ptr<SpriteFont>& font, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment)
            : _font(font), _text_alignment(text_alignment), _paragraph_alignment(paragraph_alignment)
        {
        }

        void Font::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float width, float height, const Colour& colour)
        {
            render(context, text, 0, 0, width, height, colour);
        }

        void Font::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float x, float y, float width, float height, const Colour& colour)
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

            if (_text_alignment == TextAlignment::Centre)
            {
                x += width * 0.5f - size.width * 0.5f;
            }

            if (_paragraph_alignment == ParagraphAlignment::Centre)
            {
                y += height * 0.5f - size.height * 0.5f;
            }

            _batch->Begin(SpriteSortMode_Deferred, blend_state.Get());
            _font->DrawString(_batch.get(), sanitise(*_font, text).c_str(), XMVectorSet(round(x), round(y), 0, 0), XMVectorSet(colour.b, colour.g, colour.r, colour.a), 0, XMVectorZero(), XMVectorSet(1, 1, 1, 1));
            _batch->End();
        }

        // Determines the size in pixels that the text specified will be when rendered.
        // text: The text to measure.
        // Returns: The size in pixels required.
        Size Font::measure(const std::wstring& text) const
        {
            XMFLOAT2 size;
            XMStoreFloat2(&size, _font->MeasureString(sanitise(*_font, text).c_str()));
            return Size(size.x, size.y);
        }

        bool Font::is_valid_character(wchar_t character) const
        {
            return _font->ContainsCharacter(character);
        }
    }
}
