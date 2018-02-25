#pragma once

#include <vector>
#include <cstdint>
#include <set>

#include <d3d11.h>
#include <atlbase.h>
#include <DirectXMath.h>

#include "RoomInfo.h"
#include <trview.common/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

namespace trview
{
    struct ITextureStorage;

    class Room
    {
    public:
        enum class SelectionMode
        {
            Selected,
            NotSelected,
            Neighbour
        };

        explicit Room(CComPtr<ID3D11Device> device, 
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            const ITextureStorage& texture_storage);

        RoomInfo           info() const;
        std::set<uint16_t> neighbours() const;

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, const ITextureStorage& texture_storage, SelectionMode selected);
    private:
        void generate_geometry(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ITextureStorage& texture_storage);
        void generate_adjacency(const trlevel::ILevel& level, const trlevel::tr3_room& room);

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;

        // Rendering bits:
        CComPtr<ID3D11Device>              _device;
        CComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<CComPtr<ID3D11Buffer>> _index_buffers;
        std::vector<uint32_t>              _index_counts;
        CComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                           _untextured_index_count;
        CComPtr<ID3D11Buffer>              _matrix_buffer;
        Texture                            _untextured_texture;
        DirectX::XMMATRIX                  _room_offset;
    };
}