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

namespace trview
{
    namespace graphics
    {
        class RenderTarget;
        struct IShader;
        struct IShaderStorage;
    }

    struct ICamera;
    struct ILevelTextureStorage;
    class Entity;
    class Trigger;
    class TransparencyBuffer;

    /// Draws an outline around an object.
    class SelectionRenderer final
    {
    public:
        /// Create a new SelectionRenderer.
        /// @param device The device to use to render.
        /// @param shader_storage The shader storage instance.
        explicit SelectionRenderer(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage);

        /// Render the outline around the specified object.
        /// @param context The device context.
        /// @param camera The current camera.
        /// @param texture_storage The current level texture storage instance.
        /// @param selected_item The entity to outline.
        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, Entity& selected_item);

        /// Render the outline around the specified trigger.
        /// @param context The device context.
        /// @param camera The current camera.
        /// @param texture_storage The current level texture storage instance.
        /// @param selected_item The trigger to outline.
        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, Trigger& selected_trigger);
    private:
        /// Callback for rendering solid triangles.
        using SolidCallback = std::function<void(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)>;

        /// Callback for rendering transparent triangles.
        using TransparentCallback = std::function<void(const ICamera& camera, TransparencyBuffer& transparency, const DirectX::SimpleMath::Color& colour)>;

        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const SolidCallback& solid_callback, const TransparentCallback& transparent_callback);

        /// Create vertex, index and parameter buffers.
        /// @param device The device to use to create the buffers.
        void create_buffers(const Microsoft::WRL::ComPtr<ID3D11Device>& device);

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
