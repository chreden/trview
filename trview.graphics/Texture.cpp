#include "Texture.h"

#include <vector>

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        namespace
        {
            // Convert the texture usage mode into the appropriate bind flags for a D3D texture.
            // usage: The usage mode to convert.
            // Returns: The UINT that contains the appropriate flags.
            UINT get_bind_flags(Texture::Usage usage)
            {
                UINT flags = D3D11_BIND_SHADER_RESOURCE;
                if (usage == Texture::Usage::RenderTarget)
                {
                    flags |= D3D11_BIND_RENDER_TARGET;
                }
                return flags;
            }
        }

        Texture::Texture(const ComPtr<ID3D11Texture2D>& texture, const ComPtr<ID3D11ShaderResourceView>& view)
            : texture(texture), view(view)
        {
        }

        Texture::Texture(const ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height, Usage usage)
        {
            std::vector<uint32_t> pixels(width * height, 0x00000000);

            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &pixels[0];
            srd.SysMemPitch = sizeof(uint32_t) * width;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = get_bind_flags(usage);
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            device->CreateTexture2D(&desc, &srd, &texture);
            device->CreateShaderResourceView(texture.Get(), nullptr, &view);
        }
    }
}
