/// @file SelectionRenderer.h
/// @brief Draws an outline around an object.
/// 
/// Draws an outline around an object to help the user find the object and know which one
/// is selected.

#pragma once

#include <functional>
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

#include <trview.app/Geometry/TransparencyBuffer.h>
#include <trview.graphics/RenderTarget.h>

namespace trview
{
    namespace graphics
    {
        struct IShader;
        struct IShaderStorage;
        class Device;
    }

    struct IRenderable;
    struct ICamera;
    struct ILevelTextureStorage;
    class Trigger;

    /// Draws an outline around an object.
    class SelectionRenderer final
    {
    public:
        /// Create a new SelectionRenderer.
        /// @param device The device to use to render.
        /// @param shader_storage The shader storage instance.
        explicit SelectionRenderer(const graphics::Device& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage);

        /// Render the outline around the specified object.
        /// @param context The device context.
        /// @param camera The current camera.
        /// @param texture_storage The current level texture storage instance.
        /// @param selected_item The entity to outline.
        void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, IRenderable& selected_item, const DirectX::SimpleMath::Color& outline_colour);
    private:
        /// Create vertex, index and parameter buffers.
        /// @param device The device to use to create the buffers.
        void create_buffers(const graphics::Device& device);

        std::unique_ptr<graphics::RenderTarget> _texture;
        std::unique_ptr<TransparencyBuffer> _transparency;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _index_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _scale_buffer;
        graphics::IShader* _pixel_shader;
        graphics::IShader* _vertex_shader;
    };
}
