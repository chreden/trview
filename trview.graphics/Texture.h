/// @file Texture.h
/// @brief Creates textures for different purposes.
/// 
/// Handles creation of textures and storing of textures. Can create textures that are
/// used as regular textures, depth stencil textures or render targets. This usage mode
/// is specified by the Bind enumeration.

#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>
#include <vector>
#include <trview.graphics/Device.h>
#include <trview.common/Colour.h>
#include <trview.common/Size.h>

namespace trview
{
    namespace graphics
    {
        /// Creates and stores textures to be used for different purposes.
        class Texture
        {
        public:
            /// Defines the usage of the texture. This will affect what is available and the
            /// way that the texture is created.
            enum class Bind
            {
                /// Create a regular texture. This will have a texture and a shader resource view.
                Texture,
                /// Create a render target texture. This will have a texture and a shader resource view
                /// and can be used as a render target.
                RenderTarget,
                /// Create a depth stencil texture. This will have a texture, but will not have a shader
                /// resource view. This texture can be used to create a depth stencil view.
                DepthStencil
            };

            /// Create an empty texture. This will not generate any texture or resource view but the object can 
            /// be used to store these interfaces.
            Texture() = default;

            /// Create a texture to store an existing texture and associated view. This will not generate any new resources.
            /// @param texture The texture to store.
            /// @param view The shader resource view for the texture.
            Texture(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& view);

            /// Create a texture of the specified dimensions. The optional bind mode will affect the way that this texture is created and can
            /// be used. The new texture will be filled with 0 pixels.
            /// @param device The D3D device to use to create this texture.
            /// @param width The width in pixels of the new texture.
            /// @param height The height in pixels of the new texture.
            /// @param bind An optional parameter to specify the bind mode. By default this is set to Bind::Texture.
            /// @see Bind
            Texture(const IDevice& device, uint32_t width, uint32_t height, Bind bind = Bind::Texture);

            /// Create a texture of the specified dimensions with the pixel data provided. The optional bind mode will affect the way that this
            /// texture is created and can be used.
            /// @param device The D3D device to use to create this texture.
            /// @param width The width in pixels of the new texture.
            /// @param height The height in pixels of the new texture.#
            /// @param pixels The pixel data to use to initialise the texture. This must contain at least as many elements as width x height.
            /// @param bind An optional parameter to specify the bind mode. By default this is set to Bind::Texture.
            /// @see Bind
            Texture(const IDevice& device, uint32_t width, uint32_t height, const std::vector<uint32_t>& pixels, Bind bind = Bind::Texture);

            /// Indicates whether this texture has any texture content.
            /// @returns True if the texture has content.
            bool has_content() const;

            /// Indicates whether this texture can be used as an input to a render operation.
            /// @returns True if this can be used as a shader resource.
            bool can_use_as_resource() const;

            /// Get the size of the texture.
            Size size() const;

            /// Gets the D3D texture for the texture.
            /// @returns The D3D texture.
            const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture() const;

            /// Gets the shader resource view for the texture.
            /// @returns The shader resource view.
            const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& view() const;
        private:
            /// The D3D texture that has been created or stored. Can be empty if this was created with the default constructor.
            Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture;

            /// The resource view for the D3D texture. Can be empty if this was created with the default constructor or if this texture was created
            /// with the Bind::DepthStencil bind mode.
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _view;
        };

        /// Create a texture of the specified size filled with the specified colour.
        /// @param device The device to use.
        /// @param width The width of the texture.
        /// @param height The height of the texture.
        /// @param colour The colour to fill the texture.
        Texture create_texture(const IDevice& device, uint32_t width, uint32_t height, const Colour& colour);

        /// Create a 1x1 texture filled with the specified colour.
        /// @param device The device to use.
        /// @param colour The colour to fill the texture.
        Texture create_texture(const IDevice& device, const Colour& colour);
    }
}
