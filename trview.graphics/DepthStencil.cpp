#include "DepthStencil.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        // Create a depth stencil of the specified dimensions.
        // device: The device to use to create the depth stencil.
        // width: The width of the depth stencil.
        // height: The height of the depth stencil.
        DepthStencil::DepthStencil(const graphics::IDevice& device, uint32_t width, uint32_t height)
        {
            graphics::Texture depth_stencil_buffer{ device, width, height, graphics::Texture::Bind::DepthStencil };

            D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
            memset(&depth_stencil_view_desc, 0, sizeof(depth_stencil_view_desc));

            // Set up the depth stencil view description.
            depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depth_stencil_view_desc.Texture2D.MipSlice = 0;

            // Create the depth stencil view.
            _view = device.create_depth_stencil_view(depth_stencil_buffer.texture(), depth_stencil_view_desc);
        }

        // Get the depth stencil view for the depth stencil.
        // Returns: The depth stencil view.
        ComPtr<ID3D11DepthStencilView> DepthStencil::view() const
        {
            return _view;
        }

        // Clear the depth stencil.
        // context: The device context.
        void DepthStencil::clear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
        {
            context->ClearDepthStencilView(_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
        }
    }
}