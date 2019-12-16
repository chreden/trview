#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>
#include <optional>

#include <DirectXCollision.h>
#include <SimpleMath.h>

#include <trview.app/Elements/RoomInfo.h>
#include <trview.graphics/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "StaticMesh.h"
#include <trview.app/Geometry/TransparencyBuffer.h>
#include <trview.app/Geometry/Mesh.h>
#include <trview.app/Elements/Sector.h>
#include <trview.app/Geometry/PickResult.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct IMeshStorage;
    class Entity;
    struct ICamera;
    class Mesh;
    class TransparencyBuffer;
    class Level;

    class Room
    {
    public:
        enum class SelectionMode
        {
            Selected,
            NotSelected
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

        explicit Room(const graphics::Device& device, 
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            const ILevelTextureStorage& texture_storage,
            const IMeshStorage& mesh_storage,
            uint32_t index,
            Level& parent_level);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;

        RoomInfo           info() const;
        std::set<uint16_t> neighbours() const;

        // Determine whether the specified ray hits any of the triangles in the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space.
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, bool include_entities, bool include_triggers, bool include_hidden_geometry = false, bool include_room_geometry = true) const;

        // Render the level geometry and the objects contained in this room.
        // context: The D3D context.
        // camera: The camera to use to render.
        // texture_storage: The textures for the level.
        // selected: The selection mode to use to highlight geometry and objects.
        void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected, bool show_hidden_geometry, bool show_water);

        void render_contained(const graphics::Device& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected, bool show_water, bool force_water = false);

        // Add the specified entity to the room.
        // Entity: The entity to add.
        void add_entity(Entity* entity);

        /// Add the specified trigger to the room.
        /// @paramt trigger The trigger to add.
        void add_trigger(Trigger* trigger);

        // Returns all sectors 
        const std::vector<std::shared_ptr<Sector>>& sectors() const;

        /// Add the transparent triangles to the specified transparency buffer.
        /// @param transparency The buffer to add triangles to.
        /// @param camera The current viewpoint.
        /// @param selected The current selection mode.
        /// @param include_triggers Whether to render triggers.
        void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers, bool show_water);

        // Add the transparent triangles for entities that are contained inside this room. This is called automatically
        // if get_transparent_triangles is used.
        // transparency: The buffer to add triangles to.
        // camera: The current viewpoint.
        // selected: The current selection mode.
        void get_contained_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool show_water, bool force_water = false);

        // Determines the alternate state of the room.
        AlternateMode alternate_mode() const;

        // Gets the room number of the room that is the alternate to this room.
        // If this room does not have an alternate this will be -1.
        // Returns: The room number of the alternate room.
        int16_t alternate_room() const;

        /// Gets the alternate group for the room.
        /// @returns The alternate group number.
        int16_t alternate_group() const;

        // Set this room to be the alternate room of the room specified.
        // This will change the alternate_mode of this room to IsAlternate.
        // number: The room number.
        void set_is_alternate(int16_t number);

        // Returns the number of x sectors in the room 
        inline std::uint16_t num_x_sectors() const { return _num_x_sectors; }

        // Returns the number of z sectors in the room 
        inline std::uint16_t num_z_sectors() const { return _num_z_sectors; }

        /// Get the centre point of the room.
        /// @returns The centre position of the room.
        DirectX::SimpleMath::Vector3 centre() const;

        /// Get the bounding box of the room. The bounding box is pre-transformed to the coordinates of the room.
        /// @returns The bounding box for the room.
        const DirectX::BoundingBox& bounding_box() const;

        void generate_trigger_geometry();

        uint32_t number() const;
        void update_bounding_box();

        /// Gets whether this room is a water room.
        bool water() const;
    private:
        void generate_geometry(trlevel::LevelVersion level_version, const graphics::Device& device, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage);
        void generate_adjacency();
        void generate_static_meshes(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage);
        void render_contained(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);
        void get_contained_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour);
        void generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room);
        Sector*  get_trigger_sector(int32_t x, int32_t z);
        uint32_t get_sector_id(int32_t x, int32_t z) const;

        /// Find any transparent triangles that match floor data geometry.
        /// @param transparent_triangles The transparent triangles in the sector.
        /// @param collision_triangles The collision output vector.
        void process_collision_transparency(const std::vector<TransparentTriangle>& transparent_triangles, std::vector<Triangle>& collision_triangles);

        /// Process the sectors in the level and find where there are walkable floors that have no matching geometry.
        /// @param data The room data to check against.
        /// @param room_vertices The actual room vertices.
        /// @param output_vertices Where to store vertices.
        /// @param output_indices Where to store indices.
        /// @param collision_triangles Where to store collision triangles.
        void process_unmatched_geometry(const trlevel::tr3_room_data& data, 
            const std::vector<trlevel::tr_vertex>& room_vertices,
            const std::vector<TransparentTriangle>& transparent_triangles,
            std::vector<MeshVertex>& output_vertices,
            std::vector<uint32_t>& output_indices,
            std::vector<Triangle>& collision_triangles);

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;
        uint32_t _index;

        std::vector<std::unique_ptr<StaticMesh>> _static_meshes;

        std::unique_ptr<Mesh>       _mesh;
        std::unique_ptr<Mesh>       _unmatched_mesh;
        DirectX::SimpleMath::Matrix _room_offset;

        DirectX::BoundingBox  _bounding_box;

        std::vector<Entity*> _entities;

        // Maps a sector to its sector ID 
        std::vector<std::shared_ptr<Sector>> _sectors; 

        // Number of sectors for both X and Z (required by map renderer) 
        std::uint16_t       _num_x_sectors, _num_z_sectors; 

        int16_t              _alternate_room;
        int16_t              _alternate_group;
        AlternateMode        _alternate_mode;

        std::unordered_map<uint32_t, Trigger*> _triggers;
        bool _water{ false };
        Level& _level;
    };
}