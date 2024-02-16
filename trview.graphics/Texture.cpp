#include "Texture.h"
#include <trview.common/Resources.h>
#include <external/DirectXTK/Inc/WICTextureLoader.h>
#include <trview.common/Strings.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        namespace
        {
            // Convert the texture bind mode into the appropriate bind flags for a D3D texture.
            // bind: The bind mode to convert.
            // Returns: The UINT that contains the appropriate flags.
            UINT get_bind_flags(Texture::Bind bind)
            {
                UINT flags = D3D11_BIND_SHADER_RESOURCE;
                if (bind == Texture::Bind::RenderTarget)
                {
                    flags |= D3D11_BIND_RENDER_TARGET;
                }
                else if (bind == Texture::Bind::DepthStencil)
                {
                    flags = D3D11_BIND_DEPTH_STENCIL;
                }
                return flags;
            }

            // Conver the texture bind mode into the appropriate texture format.
            // bind: The bind mode to convert.
            // Returns: The format to use.
            DXGI_FORMAT get_format(Texture::Bind bind)
            {
                if (bind == Texture::Bind::DepthStencil)
                {
                    return DXGI_FORMAT_D24_UNORM_S8_UINT;
                }
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            }
        }

        Texture::Texture(const ComPtr<ID3D11Texture2D>& texture, const ComPtr<ID3D11ShaderResourceView>& view)
            : _texture(texture), _view(view)
        {
        }

        Texture::Texture(const IDevice& device, uint32_t width, uint32_t height, Bind bind)
            : Texture(device, width, height, std::vector<uint32_t>(width * height, 0x00000000), bind)
        {
        }

        Texture::Texture(const IDevice& device, uint32_t width, uint32_t height, const std::vector<uint32_t>& pixels, Bind bind, DXGI_FORMAT format)
        {
            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &pixels[0];
            srd.SysMemPitch = sizeof(uint32_t) * width;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = format == DXGI_FORMAT_UNKNOWN ? get_format(bind) : format;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = get_bind_flags(bind);
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            _texture = device.create_texture_2D(desc, srd);
            if (bind != Texture::Bind::DepthStencil)
            {
                _view = device.create_shader_resource_view(_texture);
            }
        }

        bool Texture::has_content() const
        {
            return _texture;
        }

        bool Texture::can_use_as_resource() const
        {
            return _view;
        }

        Size Texture::size() const
        {
            D3D11_TEXTURE2D_DESC desc;
            _texture->GetDesc(&desc);
            return Size(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
        }

        const ComPtr<ID3D11Texture2D>& Texture::texture() const
        {
            return _texture;
        }

        const ComPtr<ID3D11ShaderResourceView>& Texture::view() const
        {
            return _view;
        }

        std::string Texture::name() const
        {
            return _name;
        }

        void Texture::set_name(const std::string& name)
        {
            _name = name;
        }

        Texture create_texture(const IDevice& device, uint32_t width, uint32_t height, const Colour& colour, DXGI_FORMAT format)
        {
            return Texture(device, width, height, std::vector<uint32_t>(width * height, colour), Texture::Bind::Texture, format);
        }

        Texture create_texture(const IDevice& device, uint32_t width, uint32_t height, const Colour& colour)
        {
            return Texture(device, width, height, std::vector<uint32_t>(width * height, colour));
        }

        Texture create_texture(const IDevice& device, const Colour& colour)
        {
            return create_texture(device, 1, 1, colour);
        }

        Texture load_texture_from_resource(const graphics::IDevice& device, const std::wstring& name)
        {
            using namespace Microsoft::WRL;

            ComPtr<ID3D11Resource> resource;
            ComPtr<ID3D11ShaderResourceView> view;

            auto resource_memory = get_resource_memory(name.c_str(), L"TEXTURE");
            DirectX::CreateWICTextureFromMemory(device.device().Get(), resource_memory.data, resource_memory.size, &resource, &view);

            if (!resource)
            {
                std::string error("Could not load embedded texture with ID '" + to_utf8(name) + "'");
                throw std::exception(error.c_str());
            }

            // Get the correct interface for a texture from the loaded resource.
            ComPtr<ID3D11Texture2D> texture;
            resource.As(&texture);
            return graphics::Texture{ texture, view };
        }
    }
}
