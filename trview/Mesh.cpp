#include "stdafx.h"
#include "Mesh.h"

namespace trview
{
    namespace
    {
        struct mesh_vertex
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT2 uv;
            DirectX::XMFLOAT4 colour;
        };

        // D3DXVECTOR2
        // getUv(const tr2::tr2_face4& face, std::size_t index)
        // {
        //     // Find the the point in the object map.
        //     auto tex = level_->object_texture(face.texture);
        //     auto vert = tex.vertices[index];
        //     auto tile = tex.tile;
        // 
        //     return D3DXVECTOR2(vert.xPixel / 255.0f, vert.yPixel / 255.0f);
        // }
    }

    Mesh::Mesh(const trlevel::tr_mesh& mesh, CComPtr<ID3D11Device> device)
    {
        std::vector<mesh_vertex> vertices;
        for(const auto& vert : mesh.vertices)
        {
            // pos is vert.position
            // uv is got from the level.
            // texture is got from the level too....
        }

        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(mesh_vertex) * vertices.size();
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = &vertices[0];

        HRESULT hr = device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

    }

    void Mesh::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& world_view_projection)
    {

    }
}