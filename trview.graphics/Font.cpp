#include "Font.h"
#include "Texture.h"
#include <trview.common/Size.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        Font::Font(const ComPtr<ID3D11Device>& device, const ComPtr<IDWriteFactory>& dwrite_factory, const ComPtr<ID2D1Factory>& d2d_factory, const ComPtr<IDWriteTextFormat>& text_format)
            : _device(device), _dwrite_factory(dwrite_factory), _d2d_factory(d2d_factory), _text_format(text_format)
        {
        }

        FontTexture Font::create_texture(const graphics::Texture& texture)
        {
            ComPtr<IDXGISurface> surface;
            texture.texture().As(&surface);

            D2D1_RENDER_TARGET_PROPERTIES props =
                D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT,
                    D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                    96,
                    96
                );

            FontTexture new_texture;
            _d2d_factory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, &new_texture.render_target);
            new_texture.render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &new_texture.brush);
            return new_texture;
        }

        void Font::render(FontTexture& texture, const std::wstring& text, float x, float y, float width, float height)
        {
            D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, width, height);

            texture.render_target->BeginDraw();
            texture.render_target->SetTransform(D2D1::Matrix3x2F::Translation(x, y));
            texture.render_target->DrawText(text.c_str(), static_cast<uint32_t>(text.size()), _text_format.Get(), layoutRect, texture.brush.Get());
            texture.render_target->EndDraw();
        }

        // Determines the size in pixels that the text specified will be when rendered.
        // text: The text to measure.
        // Returns: The size in pixels required.
        Size Font::measure(const std::wstring& text) const
        {
            // Create a text layout from the factory (which we don't have...)
            ComPtr<IDWriteTextLayout> text_layout;
            _dwrite_factory->CreateTextLayout(text.c_str(), static_cast<uint32_t>(text.size()), _text_format.Get(), 10000, 10000, &text_layout);
            DWRITE_TEXT_METRICS metrics;
            text_layout->GetMetrics(&metrics);
            return Size(std::ceil(metrics.width), std::ceil(metrics.height));
        }
    }
}
