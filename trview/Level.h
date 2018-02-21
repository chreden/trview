#pragma once

#include <memory>
#include <atlbase.h>
#include <d3d11.h>
#include <vector>
#include <unordered_map>

#include <trview.common/Texture.h>
#include <trlevel/ILevel.h>

#include "Room.h"
#include "Entity.h"
#include "Mesh.h"
#include "StaticMesh.h"

namespace trview
{
    class Level
    {
    public:
        Level(CComPtr<ID3D11Device> device, const trlevel::ILevel* level);

        enum class RoomHighlightMode
        {
            None,
            Highlight,
            Neighbours
        };

        // Temporary, for the room info and texture window.

        std::vector<RoomInfo> room_info() const;
        std::vector<Texture> level_textures() const;

        void render(CComPtr<ID3D11DeviceContext> context, DirectX::XMMATRIX view_projection);
        
        RoomHighlightMode highlight_mode() const;
        void set_highlight_mode(RoomHighlightMode mode);
        void set_selected_room(uint16_t index);
        void set_neighbour_depth(uint32_t depth);
    private:
        // Generate the textures that will be used to render the level based on the
        // textiles loaded in the level.
        void generate_textures();
        void generate_rooms();
        void generate_static_meshes();
        void generate_entities();
        void regenerate_neighbours();
        void generate_neighbours(std::set<uint16_t>& all_rooms, uint16_t previous_room, uint16_t selected_room, int32_t current_depth, int32_t max_depth);

        const trlevel::ILevel*               _level;
        std::vector<Texture>                 _textures;
        std::vector<std::unique_ptr<Room>>   _rooms;
        std::vector<std::unique_ptr<Entity>> _entities;

        CComPtr<ID3D11Device>       _device;
        CComPtr<ID3D11VertexShader> _vertex_shader;
        CComPtr<ID3D11PixelShader>  _pixel_shader;
        CComPtr<ID3D11InputLayout>  _input_layout;
        CComPtr<ID3D11SamplerState> _sampler_state;

        RoomHighlightMode  _room_highlight_mode{ RoomHighlightMode::None };
        uint16_t           _selected_room{ 0u };
        uint32_t           _neighbour_depth{ 1 };
        std::set<uint16_t> _neighbours;

        // Mesh storage.
        Mesh* get_mesh(uint32_t mesh_pointer);

        std::unordered_map<uint16_t, std::unique_ptr<Mesh>> _meshes;
        std::vector<std::unique_ptr<StaticMesh>> _static_meshes;
    };
}
