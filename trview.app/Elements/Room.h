#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>
#include <optional>

#include <DirectXCollision.h>
#include <SimpleMath.h>

#include <trview.graphics/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "StaticMesh.h"
#include <trview.app/Geometry/TransparencyBuffer.h>
#include <trview.app/Geometry/IMesh.h>
#include <trview.app/Elements/Sector.h>
#include <trview.app/Geometry/PickResult.h>
#include "IRoom.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct IMeshStorage;
    struct IEntity;
    struct ICamera;
    class Mesh;
    class TransparencyBuffer;
    struct ILevel;

    class Room : public IRoom
    {
    public:
        explicit Room(const IMesh::Source& mesh_source,
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            const ILevelTextureStorage& texture_storage,
            const IMeshStorage& mesh_storage,
            uint32_t index,
            const ILevel& parent_level);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;

        virtual RoomInfo info() const override;
        virtual std::set<uint16_t> neighbours() const override;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, bool include_entities, bool include_triggers, bool include_hidden_geometry = false, bool include_room_geometry = true) const override;

        // Render the level geometry and the objects contained in this room.
        // camera: The camera to use to render.
        // texture_storage: The textures for the level.
        // selected: The selection mode to use to highlight geometry and objects.
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected, bool show_hidden_geometry, bool show_water) override;

        virtual void render_contained(const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected, bool show_water, bool force_water = false) override;

        // Add the specified entity to the room.
        // Entity: The entity to add.
        virtual void add_entity(IEntity* entity) override;

        /// Add the specified trigger to the room.
        /// @paramt trigger The trigger to add.
        virtual void add_trigger(Trigger* trigger) override;

        virtual const std::vector<std::shared_ptr<Sector>> sectors() const override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers, bool show_water) override;

        virtual void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool show_water, bool force_water = false) override;

        virtual AlternateMode alternate_mode() const override;
        virtual int16_t alternate_room() const override;
        virtual int16_t alternate_group() const override;

        // Set this room to be the alternate room of the room specified.
        // This will change the alternate_mode of this room to IsAlternate.
        // number: The room number.
        virtual void set_is_alternate(int16_t number) override;

        virtual uint16_t num_x_sectors() const override;
        virtual uint16_t num_z_sectors() const override;

        virtual DirectX::SimpleMath::Vector3 centre() const override;

        virtual DirectX::BoundingBox bounding_box() const override;

        virtual void generate_trigger_geometry() override;

        virtual uint32_t number() const override;
        virtual void update_bounding_box() override;

        virtual bool outside() const override;

        virtual bool water() const override;

        virtual bool quicksand() const override;

        virtual Trigger* trigger_at(int32_t x, int32_t z) const override;
    private:
        void generate_geometry(trlevel::LevelVersion level_version, const IMesh::Source& mesh_source, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage);
        void generate_adjacency();
        void generate_static_meshes(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage);
        void render_contained(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);
        void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour);
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

        std::unique_ptr<IMesh> _mesh;
        std::unique_ptr<IMesh> _unmatched_mesh;
        DirectX::SimpleMath::Matrix _room_offset;

        DirectX::BoundingBox  _bounding_box;

        std::vector<IEntity*> _entities;

        // Maps a sector to its sector ID 
        std::vector<std::shared_ptr<Sector>> _sectors; 

        // Number of sectors for both X and Z (required by map renderer) 
        std::uint16_t       _num_x_sectors, _num_z_sectors; 

        int16_t              _alternate_room;
        int16_t              _alternate_group;
        AlternateMode        _alternate_mode;

        std::unordered_map<uint32_t, Trigger*> _triggers;
        uint16_t _flags{ 0 };
        const ILevel& _level;
    };
}