#include "Font.h"
#include "Texture.h"
#include <trview.common/Size.h>
#include <trview.common/Colour.h>
#include <sstream>
#include <external/DirectXTK/Inc/SpriteBatch.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        Font::Font(const ComPtr<ID3D11Device>& device, float size, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment)
            : _font(device.Get(), L"C:\\Projects\\Applications\\trview\\font.bin")
        {
        }

        void Font::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float x, float y, float width, float height)
        {
            DirectX::SpriteBatch batch(context.Get());
            batch.Begin();
            _font.DrawString(&batch, text.c_str(), DirectX::XMVectorSet(x, y, 0, 0), DirectX::XMVectorSet(1, 1, 1, 1), 0, DirectX::XMVectorZero(), DirectX::XMVectorSet(1, 1, 1, 1));
            batch.End();
        }

        // Determines the size in pixels that the text specified will be when rendered.
        // text: The text to measure.
        // Returns: The size in pixels required.
        Size Font::measure(const std::wstring& text) const
        {
            DirectX::XMFLOAT2 size;
            DirectX::XMStoreFloat2(&size, _font.MeasureString(text.c_str()));
            return Size(size.x, size.y);
        }
    }
}
