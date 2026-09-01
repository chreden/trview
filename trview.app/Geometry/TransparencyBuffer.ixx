module;

#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

export module trview.app:TransparencyBuffer;

import std;

import trview.graphics;

import :ITransparencyBuffer;
import :ITextureStorage;
import :MeshVertex;

namespace trview
{
    // Collects transparent triangles to be rendered and provides
    // the buffers required for rendering.
    export class TransparencyBuffer final : public ITransparencyBuffer
    {
    public:
        explicit TransparencyBuffer(const std::shared_ptr<graphics::IDevice>& device, const std::weak_ptr<ITextureStorage>& level_texture_storage);
        TransparencyBuffer(const TransparencyBuffer&) = delete;
        TransparencyBuffer& operator=(const TransparencyBuffer&) = delete;

        // Add a triangle to the transparency buffer. The triangle will be added to the end
        // of the collection.
        // triangle: The triangle to add.
        void add(const Triangle& triangle) override;

        // Sort the accumulated transparent triangles in order of farthest to
        // nearest, based on the position of the camera.
        // eye_position: The position of the camera.
        void sort(const DirectX::SimpleMath::Vector3& eye_position) override;

        /// Render the accumulated transparent triangles. Sort should be called before this function is called.
        /// @param camera The current camera.
        /// @param texture_storage Texture storage for the level.
        /// @param ignore_blend Optional. Set to true to render this without transparency.
        void render(const ICamera& camera, bool ignore_blend = false) override;

        // Reset the triangles buffer.
        void reset() override;
    private:
        void create_buffer();
        void create_matrix_buffer();
        void complete();
        void set_blend_mode(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, Triangle::TransparencyMode mode) const;

        std::shared_ptr<graphics::IDevice> _device;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11BlendState> _alpha_blend;
        Microsoft::WRL::ComPtr<ID3D11BlendState> _additive_blend;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _transparency_depth_state;

        std::vector<Triangle> _triangles;
        std::vector<MeshVertex> _vertices;

        struct TextureRun
        {
            uint32_t texture;
            Triangle::TextureMode texture_mode;
            Triangle::TransparencyMode transparency_mode;
            uint32_t count;
        };

        std::vector<TextureRun> _texture_run;
        graphics::Texture _untextured;
        std::weak_ptr<ITextureStorage> _texture_storage;
    };
}
