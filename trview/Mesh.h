#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include <trlevel/trtypes.h>

#include "MeshVertex.h"

namespace trview
{
    struct ILevelTextureStorage;

    class Mesh
    {
    public:
        // Create a mesh using the specified vertices and indices.
        // device: The D3D device to create the mesh.
        // vertices: The vertices that make up the mesh.
        // indices: The indices for triangles that use level textures.
        // untextured_indices: The indices for triangles that do not use level textures.
        // texture_storage: The texture storage for the level.
        Mesh(CComPtr<ID3D11Device> device,
             const std::vector<MeshVertex>& vertices, 
             const std::vector<std::vector<uint32_t>>& indices, 
             const std::vector<uint32_t>& untextured_indices,
             const ILevelTextureStorage& texture_storage);

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::SimpleMath::Matrix& world_view_projection, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);
    private:
        CComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<uint32_t>              _index_counts;
        std::vector<CComPtr<ID3D11Buffer>> _index_buffers;
        CComPtr<ID3D11Buffer>              _matrix_buffer;
        CComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                           _untextured_index_count;
    };

    // Create a new mesh based on the contents of the mesh specified.
    // mesh: The level mesh to generate.
    // device: The D3D device to use to create the mesh.
    // texture_storage: The textures for the level.
    // Returns: The new mesh.
    std::unique_ptr<Mesh> create_mesh(const trlevel::tr_mesh& mesh, CComPtr<ID3D11Device> device, const ILevelTextureStorage& texture_storage);
}