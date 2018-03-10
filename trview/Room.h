#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>

#include <d3d11.h>
#include <atlbase.h>
#include <DirectXMath.h>

#include "RoomInfo.h"
#include <trview.common/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "StaticMesh.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct IMeshStorage;

    class Room
    {
    public:
        enum class SelectionMode
        {
            Selected,
            NotSelected,
            Neighbour
        };

        struct Triangle
        {
            DirectX::XMVECTOR v0;
            DirectX::XMVECTOR v1;
            DirectX::XMVECTOR v2;
        };

        explicit Room(CComPtr<ID3D11Device> device, 
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            const ILevelTextureStorage& texture_storage,
            const IMeshStorage& mesh_storage);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;

        RoomInfo           info() const;
        std::set<uint16_t> neighbours() const;

        const std::vector<Triangle>& collision_triangles() const;

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, const ILevelTextureStorage& texture_storage, SelectionMode selected);
    private:
        void generate_geometry(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage);
        void generate_adjacency(const trlevel::ILevel& level, const trlevel::tr3_room& room);
        void generate_static_meshes(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage);

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;

        std::vector<std::unique_ptr<StaticMesh>> _static_meshes;

        // Rendering bits:
        CComPtr<ID3D11Device>              _device;
        CComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<CComPtr<ID3D11Buffer>> _index_buffers;
        std::vector<uint32_t>              _index_counts;
        CComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                           _untextured_index_count;
        CComPtr<ID3D11Buffer>              _matrix_buffer;
        DirectX::XMMATRIX                  _room_offset;

        // Triangle copy for ray intersection.
        std::vector<Triangle> _collision_triangles;
    };
}