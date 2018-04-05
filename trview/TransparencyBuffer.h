#pragma once

#include <vector>
#include <SimpleMath.h>
#include <atlbase.h>
#include <d3d11.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;

    struct TransparentTriangle
    {
        TransparentTriangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2,
            const DirectX::SimpleMath::Vector2& uv0, const DirectX::SimpleMath::Vector2& uv1, const DirectX::SimpleMath::Vector2& uv2,
            uint32_t texture)
            : vertices{ v0, v1, v2 }, uvs{ uv0, uv1, uv2 }, texture(texture)
        {
        }

        TransparentTriangle transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour) const;

        // The world space positions that make up the triangle.
        DirectX::SimpleMath::Vector3 vertices[3];
        // UV coordinates for the triangle.
        DirectX::SimpleMath::Vector2 uvs[3];
        // The world space centre position of the three vertices.
        DirectX::SimpleMath::Vector3 position;
        // The level texture index to use.
        uint32_t                     texture;

        DirectX::SimpleMath::Color   colour{ 1, 1, 1, 1 };
    };

    // Collects transparent triangles to be rendered and provides
    // the buffers required for rendering.
    class TransparencyBuffer
    {
    public:
        TransparencyBuffer() = default;
        TransparencyBuffer(const TransparencyBuffer&) = delete;
        TransparencyBuffer& operator=(const TransparencyBuffer&) = delete;

        void add(const TransparentTriangle& triangle);

        // Sort the accumulated transparent triangles in order of farthest to
        // nearest, based on the position of the camera.
        void sort(const DirectX::SimpleMath::Vector3& eye_position);

        void render(CComPtr<ID3D11Device> device, 
            CComPtr<ID3D11DeviceContext> context, 
            const ICamera& camera, 
            const ILevelTextureStorage& texture_storage);
    private:
        std::vector<TransparentTriangle> _triangles;
    };
}
