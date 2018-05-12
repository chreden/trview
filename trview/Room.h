#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include "RoomInfo.h"
#include <trview.graphics/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "StaticMesh.h"
#include "TransparencyBuffer.h"
#include "Mesh.h"
#include <trview\Sector.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct IMeshStorage;
    class Entity;
    struct ICamera;
    class Mesh;
    class TransparencyBuffer;

    class Room
    {
    public:
        enum class SelectionMode
        {
            Selected,
            NotSelected,
            Neighbour
        };

        enum class AlternateMode
        {
            // This room does not have an alternate room and is not an alternate room to another room.
            None,
            // This room has an alternate room.
            HasAlternate,
            // This room is an alternate room to another room.
            IsAlternate
        };

        struct PickResult
        {
            bool                         hit{ false };
            float                        distance;
            DirectX::SimpleMath::Vector3 position;
        };

        explicit Room(const Microsoft::WRL::ComPtr<ID3D11Device>& device, 
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            const ILevelTextureStorage& texture_storage,
            const IMeshStorage& mesh_storage);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;

        RoomInfo           info() const;
        std::set<uint16_t> neighbours() const;

        // Determine whether the specified ray hits any of the triangles in the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space.
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;

        // Render the level geometry and the objects contained in this room.
        // context: The D3D context.
        // camera: The camera to use to render.
        // texture_storage: The textures for the level.
        // selected: The selection mode to use to highlight geometry and objects.
        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected);

        void render_contained(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected);

        // Add the specified entity to the room.
        // Entity: The entity to add.
        void add_entity(Entity* entity);

        // Returns all sectors 
        const inline std::map<int, std::shared_ptr<Sector>>& sectors() const { return _sectors; }

        // Add the transparent triangles to the specified transparency buffer.
        // transparency: The buffer to add triangles to.
        // camera: The current viewpoint.
        // selected: The current selection mode.
        void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected);

        // Add the transparent triangles for entities that are contained inside this room. This is called automatically
        // if get_transparent_triangles is used.
        // transparency: The buffer to add triangles to.
        // camera: The current viewpoint.
        // selected: The current selection mode.
        void get_contained_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected);

        // Determines the alternate state of the room.
        AlternateMode alternate_mode() const;

        // Gets the room number of the room that is the alternate to this room.
        // If this room does not have an alternate this will be -1.
        // Returns: The room number of the alternate room.
        int16_t alternate_room() const;

        // Set this room to be the alternate room of the room specified.
        // This will change the alternate_mode of this room to IsAlternate.
        // number: The room number.
        void set_is_alternate(int16_t number);

        // Returns the number of x sectors in the room 
        inline std::uint16_t num_x_sectors() const { return _num_x_sectors; }

        // Returns the number of z sectors in the room 
        inline std::uint16_t num_z_sectors() const { return _num_z_sectors; }


    private:
        void generate_geometry(const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage);
        void generate_adjacency();
        void generate_static_meshes(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage);
        void render_contained(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);
        void get_contained_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour);
        void generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room);

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;

        std::vector<std::unique_ptr<StaticMesh>> _static_meshes;

        Microsoft::WRL::ComPtr<ID3D11Device>       _device;
        std::unique_ptr<Mesh>       _mesh;
        DirectX::SimpleMath::Matrix _room_offset;

        // Triangle copy for ray intersection.
        std::vector<Triangle> _collision_triangles;
        DirectX::BoundingBox  _bounding_box;

        std::vector<Entity*> _entities;

        // Maps a sector to its sector ID 
        std::map<int, std::shared_ptr<Sector>> _sectors; 

        // Number of sectors for both X and Z (required by map renderer) 
        std::uint16_t       _num_x_sectors, _num_z_sectors; 

        int16_t              _alternate_room;
        AlternateMode        _alternate_mode;
    };
}