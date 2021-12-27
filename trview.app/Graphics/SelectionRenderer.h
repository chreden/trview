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

#include <trview.app/Geometry/ITransparencyBuffer.h>
#include <trview.graphics/IRenderTarget.h>
#include "ISelectionRenderer.h"

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

    /// Draws an outline around an object.
    class SelectionRenderer final : public ISelectionRenderer
    {
    public:
        /// Create a new SelectionRenderer.
        /// @param device The device to use to render.
        /// @param shader_storage The shader storage instance.
        explicit SelectionRenderer(
            const std::shared_ptr<graphics::IDevice>& device,
            const std::shared_ptr<graphics::IShaderStorage>& shader_storage,
            std::unique_ptr<ITransparencyBuffer> transparency,
            const graphics::IRenderTarget::SizeSource& render_target_source);

        /// Render the outline around the specified object.
        /// @param camera The current camera.
        /// @param selected_item The entity to outline.
        void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, IRenderable& selected_item, const DirectX::SimpleMath::Color& outline_colour);
    private:
        /// Create vertex, index and parameter buffers.
        /// @param device The device to use to create the buffers.
        void create_buffers(const graphics::IDevice& device);

        std::shared_ptr<graphics::IDevice> _device;
        std::unique_ptr<graphics::IRenderTarget> _texture;
        graphics::IRenderTarget::SizeSource _render_target_source;
        std::unique_ptr<ITransparencyBuffer> _transparency;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _index_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _scale_buffer;
        graphics::IShader* _pixel_shader;
        graphics::IShader* _vertex_shader;
    };
}
