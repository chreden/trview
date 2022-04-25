#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>
#include <optional>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>
#include <trview.app/Geometry/IMesh.h>
#include <trview.app/Elements/ISector.h>
#include <trview.app/Geometry/PickResult.h>
#include <trview.graphics/Texture.h>
#include "IStaticMesh.h"
#include "IRoom.h"

#include <trview.common/TokenStore.h>

namespace trview
{
    class Room final : public IRoom, public std::enable_shared_from_this<IRoom>
    {
    public:
        explicit Room(const IMesh::Source& mesh_source,
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            std::shared_ptr<ILevelTextureStorage> texture_storage,
            const IMeshStorage& mesh_storage,
            uint32_t index,
            const ILevel& parent_level,
            const IStaticMesh::MeshSource& static_mesh_mesh_source,
            const IStaticMesh::PositionSource& static_mesh_position_source,
            const ISector::Source& sector_source);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;
        virtual RoomInfo info() const override;
        virtual std::set<uint16_t> neighbours() const override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, PickFilter filters = PickFilter::Default) const override;
        virtual void render(const ICamera& camera, SelectionMode selected, bool show_hidden_geometry, bool show_water, bool use_trle_colours, const std::unordered_set<uint32_t>& visible_rooms) override;
        virtual void render_bounding_boxes(const ICamera& camera) override;
        virtual void render_lights(const ICamera& camera, const std::weak_ptr<ILight>& selected_light) override;
        virtual void render_contained(const ICamera& camera, SelectionMode selected, bool show_water) override;
        virtual void add_entity(const std::weak_ptr<IEntity>& entity) override;
        virtual void add_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void add_light(const std::weak_ptr<ILight>& light) override;
        virtual const std::vector<std::shared_ptr<ISector>> sectors() const override;
        virtual void generate_sector_triangles() override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers, bool show_water, bool trle_mode) override;
        virtual void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool show_water) override;
        virtual AlternateMode alternate_mode() const override;
        virtual int16_t alternate_room() const override;
        virtual int16_t alternate_group() const override;
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
        virtual std::weak_ptr<ITrigger> trigger_at(int32_t x, int32_t z) const override;
        virtual bool flag(Flag flag) const override;
        virtual float y_bottom() const override;
        virtual float y_top() const override;
        virtual ISector::Portal sector_portal(int x1, int y1, int x2, int z2) const override;
        virtual void set_sector_triangle_rooms(const std::vector<uint32_t>& triangles) override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
    private:
        void generate_geometry(trlevel::LevelVersion level_version, const IMesh::Source& mesh_source, const trlevel::tr3_room& room);
        void generate_adjacency();
        void generate_static_meshes(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage,
            const IStaticMesh::MeshSource& static_mesh_mesh_source, const IStaticMesh::PositionSource& static_mesh_position_source);
        void render_contained(const ICamera& camera, const DirectX::SimpleMath::Color& colour);
        void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour);
        void generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ISector::Source& sector_source);
        ISector* get_trigger_sector(int32_t x, int32_t z);
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

        void generate_trle_mesh(const IMesh::Source& mesh_source);

        void add_centroid_to_pick(const IMesh& mesh, PickResult& geometry_result) const;

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;
        uint32_t _index;

        std::vector<std::shared_ptr<IStaticMesh>> _static_meshes;

        std::shared_ptr<IMesh> _mesh;
        std::shared_ptr<IMesh> _unmatched_mesh;
        std::unordered_map<uint32_t, std::shared_ptr<IMesh>> _trle_meshes;
        DirectX::SimpleMath::Matrix _room_offset;
        DirectX::SimpleMath::Matrix _inverted_room_offset;

        DirectX::BoundingBox  _bounding_box;

        std::vector<std::weak_ptr<IEntity>> _entities;

        // Maps a sector to its sector ID 
        std::vector<std::shared_ptr<ISector>> _sectors; 

        // Number of sectors for both X and Z (required by map renderer) 
        std::uint16_t       _num_x_sectors, _num_z_sectors; 

        int16_t              _alternate_room;
        int16_t              _alternate_group;
        AlternateMode        _alternate_mode;

        TokenStore _token_store;
        std::unordered_map<uint32_t, std::weak_ptr<ITrigger>> _triggers;
        uint16_t _flags{ 0 };
        const ILevel& _level;
        std::shared_ptr<ILevelTextureStorage> _texture_storage;
        std::vector<std::weak_ptr<ILight>> _lights;
        IMesh::Source _mesh_source;
        std::vector<uint32_t> _trle_sector_rooms;
        std::function<std::shared_ptr<IMesh>()> _unmatched_mesh_generator;
    };
}