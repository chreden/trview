#include "Font.h"
#include <trview.ui/Size.h>

#include <d2d1.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            Font::Font(
                CComPtr<ID3D11Device> device,
                CComPtr<IDWriteFactory> dwrite_factory,
                CComPtr<ID2D1Factory> d2d_factory,
                CComPtr<IDWriteTextFormat> text_format)
                : _device(device), _dwrite_factory(dwrite_factory), _d2d_factory(d2d_factory), _text_format(text_format)
            {
            }

            FontTexture Font::create_texture(CComPtr<ID3D11Texture2D> texture)
            {
                CComPtr<IDXGISurface> surface;
                texture->QueryInterface(&surface);

                D2D1_RENDER_TARGET_PROPERTIES props =
                    D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                        96,
                        96
                    );

                FontTexture new_texture;
                _d2d_factory->CreateDxgiSurfaceRenderTarget(surface, &props, &new_texture.render_target);
                new_texture.render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &new_texture.brush);
                return new_texture;
            }

            FontTexture Font::create_texture()
            {
                FontTexture new_texture;
                D3D11_TEXTURE2D_DESC desc;
                desc.ArraySize = 1;
                desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = 0;
                desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                desc.Height = 512;
                desc.Width = 512;
                desc.MipLevels = 1;
                desc.MiscFlags = 0;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Usage = D3D11_USAGE_DEFAULT;
                _device->CreateTexture2D(&desc, nullptr, &new_texture.texture);

                CComPtr<IDXGISurface> surface;
                new_texture.texture->QueryInterface(&surface);

                D2D1_RENDER_TARGET_PROPERTIES props =
                    D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_IGNORE),
                        96,
                        96
                    );

                _d2d_factory->CreateDxgiSurfaceRenderTarget(surface, &props, &new_texture.render_target);
                new_texture.render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &new_texture.brush);
                _device->CreateShaderResourceView(new_texture.texture, nullptr, &new_texture.view);
                return new_texture;
            }

            void Font::render(FontTexture& texture, const std::wstring& text, float x, float y, float width, float height)
            {
                D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, width, height);

                texture.render_target->BeginDraw();
                texture.render_target->SetTransform(D2D1::Matrix3x2F::Translation(x, y));
                texture.render_target->DrawText(text.c_str(), text.size(), _text_format, layoutRect, texture.brush);
                texture.render_target->EndDraw();
            }

            // Determines the size in pixels that the text specified will be when rendered.
            // text: The text to measure.
            // Returns: The size in pixels required.
            Size Font::measure(const std::wstring& text) const
            {
                // Create a text layout from the factory (which we don't have...)
                CComPtr<IDWriteTextLayout> text_layout;
                _dwrite_factory->CreateTextLayout(text.c_str(), static_cast<uint32_t>(text.size()), _text_format, 10000, 10000, &text_layout);
                DWRITE_TEXT_METRICS metrics;
                text_layout->GetMetrics(&metrics);
                return ui::Size(std::ceil(metrics.width), std::ceil(metrics.height));
            }
        }
    }
}
