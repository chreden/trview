#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <trlevel/trtypes.h>

namespace trview
{
    struct ILevelTextureStorage;

    class Mesh
    {
    public:
        explicit Mesh(const trlevel::tr_mesh& mesh, CComPtr<ID3D11Device> device, const ILevelTextureStorage& texture_storage);

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& world_view_projection, const ILevelTextureStorage& texture_storage, const DirectX::XMFLOAT4& colour);
    private:
        CComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<uint32_t>              _index_counts;
        std::vector<CComPtr<ID3D11Buffer>> _index_buffers;
        CComPtr<ID3D11Buffer>              _matrix_buffer;
        CComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                           _untextured_index_count;
    };
}