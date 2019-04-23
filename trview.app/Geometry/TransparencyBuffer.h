#pragma once

#include <vector>
#include <SimpleMath.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <trview.app/Geometry/MeshVertex.h>
#include <trview.app/Geometry/TransparentTriangle.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/Texture.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;

    // Collects transparent triangles to be rendered and provides
    // the buffers required for rendering.
    class TransparencyBuffer
    {
    public:
        explicit TransparencyBuffer(const graphics::Device& device);
        TransparencyBuffer(const TransparencyBuffer&) = delete;
        TransparencyBuffer& operator=(const TransparencyBuffer&) = delete;

        // Add a triangle to the transparency buffer. The triangle will be added to the end
        // of the collection.
        // triangle: The triangle to add.
        void add(const TransparentTriangle& triangle);

        // Sort the accumulated transparent triangles in order of farthest to
        // nearest, based on the position of the camera.
        // eye_position: The position of the camera.
        void sort(const DirectX::SimpleMath::Vector3& eye_position);

        /// Render the accumulated transparent triangles. Sort should be called before this function is called.
        /// @param context Current device context.
        /// @param camera The current camera.
        /// @param texture_storage Texture storage for the level.
        /// @param ignore_blend Optional. Set to true to render this without transparency.
        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, bool ignore_blend = false);

        // Reset the triangles buffer.
        void reset();
    private:
        void create_buffer();
        void create_matrix_buffer();
        void complete();
        void set_blend_mode(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, TransparentTriangle::Mode mode) const;

        const graphics::Device& _device;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11BlendState> _alpha_blend;
        Microsoft::WRL::ComPtr<ID3D11BlendState> _additive_blend;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _transparency_depth_state;

        std::vector<TransparentTriangle> _triangles;
        std::vector<MeshVertex> _vertices;

        struct TextureRun
        {
            uint32_t texture;
            TransparentTriangle::Mode mode;
            uint32_t count;
        };

        std::vector<TextureRun> _texture_run;
        graphics::Texture _untextured;
    };
}
