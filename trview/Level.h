#pragma once

#include <memory>
#include <atlbase.h>
#include <d3d11.h>
#include <vector>
#include <unordered_map>
#include <SimpleMath.h>

#include <trview.common/Texture.h>
#include <trlevel/ILevel.h>

#include "Room.h"
#include "Entity.h"
#include "Mesh.h"
#include "StaticMesh.h"

#include "IMeshStorage.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;

    class Level
    {
    public:
        Level(CComPtr<ID3D11Device> device, const trlevel::ILevel* level);
        ~Level();

        enum class RoomHighlightMode
        {
            None,
            Highlight,
            Neighbours
        };

        struct PickResult
        {
            bool                         hit{ false };
            uint32_t                     room;
            DirectX::SimpleMath::Vector3 position;
            float                        distance{ FLT_MAX };
        };

        // Temporary, for the room info and texture window.

        std::vector<RoomInfo> room_info() const;
        RoomInfo room_info(uint32_t room) const;
        std::vector<Texture> level_textures() const;

        uint16_t selected_room() const;

        // Determine whether the specified ray hits any of the triangles in any of the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space. The room that was hit
        // is also specified.
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;

        void render(CComPtr<ID3D11DeviceContext> context, const ICamera& camera);

        RoomHighlightMode highlight_mode() const;
        void set_highlight_mode(RoomHighlightMode mode);
        void set_selected_room(uint16_t index);
        void set_neighbour_depth(uint32_t depth);
        void on_camera_moved();
    private:
        void generate_rooms();
        void generate_entities();
        void regenerate_neighbours();
        void generate_neighbours(std::set<uint16_t>& all_rooms, uint16_t previous_room, uint16_t selected_room, int32_t current_depth, int32_t max_depth);

        // Render the rooms in the level.
        // context: The device context.
        // camera: The current camera to render the level with.
        void render_rooms(CComPtr<ID3D11DeviceContext> context, const ICamera& camera);

        struct RoomToRender
        {
            RoomToRender(Room* room, Room::SelectionMode selection_mode)
                : room(room), selection_mode(selection_mode)
            {
            }

            Room*               room{ nullptr };
            Room::SelectionMode selection_mode;
        };

        // Get the collection of rooms that need to be renderered depending on the current view mode.
        // Returns: The rooms to render and their selection mode.
        std::vector<RoomToRender> get_rooms_to_render() const;

        // Determines whether the room is currently being rendered.
        // room: The room index.
        // Returns: True if the room is visible.
        bool room_visible(uint32_t room) const;

        const trlevel::ILevel*               _level;
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

        std::unique_ptr<ILevelTextureStorage> _texture_storage;
        std::unique_ptr<IMeshStorage> _mesh_storage;
        std::unique_ptr<TransparencyBuffer> _transparency;

        bool _regenerate_transparency{ true };
    };
}
