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
#include <trview.common/Logs/ILog.h>

#include <trview.graphics/Sampler/ISamplerState.h>

namespace trview
{
    struct Sampler
    {
    public:
        void apply();
    };

    class Room final : public IRoom, public std::enable_shared_from_this<IRoom>
    {
    public:
        explicit Room(const trlevel::tr3_room& room,
            const IMesh::Source& mesh_source,
            std::shared_ptr<ILevelTextureStorage> texture_storage,
            uint32_t index,
            const std::weak_ptr<ILevel>& parent_level,
            const graphics::ISamplerState::Source& sampler_source);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;
        virtual RoomInfo info() const override;
        virtual std::set<uint16_t> neighbours() const override;
        virtual std::vector<PickResult> pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, PickFilter filters = PickFilter::Default) const override;
        virtual void render(const ICamera& camera, SelectionMode selected, RenderFilter render_filter, const std::unordered_set<uint32_t>& visible_rooms) override;
        virtual void render_bounding_boxes(const ICamera& camera) override;
        virtual void render_lights(const ICamera& camera, const std::weak_ptr<ILight>& selected_light) override;
        virtual void render_camera_sinks(const ICamera& camera) override;
        virtual void render_contained(const ICamera& camera, SelectionMode selected, RenderFilter render_filter) override;
        virtual void add_entity(const std::weak_ptr<IItem>& entity) override;
        virtual void add_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void add_light(const std::weak_ptr<ILight>& light) override;
        virtual void add_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        std::weak_ptr<ISector> sector(int32_t x, int32_t z) const override;
        DirectX::SimpleMath::Vector3 sector_centroid(const std::weak_ptr<ISector>& sector) const override;
        virtual std::vector<std::shared_ptr<ISector>> sectors() const override;
        virtual void generate_sector_triangles() override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, RenderFilter render_filter) override;
        virtual void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, RenderFilter render_filter) override;
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
        uint16_t flags() const override;
        virtual float y_bottom() const override;
        virtual float y_top() const override;
        virtual ISector::Portal sector_portal(int x1, int y1, int x2, int z2) const override;
        virtual void set_sector_triangle_rooms(const std::vector<uint32_t>& triangles) override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual bool visible() const override;
        virtual void set_visible(bool visible) override;
        virtual std::vector<std::weak_ptr<ILight>> lights() const override;
        virtual std::vector<std::weak_ptr<ICameraSink>> camera_sinks() const override;
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const override;
        std::vector<std::weak_ptr<IItem>> items() const override;
        std::weak_ptr<ILevel> level() const override;
        Colour ambient() const override;
        int16_t ambient_intensity_1() const override;
        int16_t ambient_intensity_2() const override;
        int16_t light_mode() const override;
        void initialise(const trlevel::ILevel& level,
            const trlevel::tr3_room& room,
            const IMeshStorage& mesh_storage,
            const IStaticMesh::MeshSource& static_mesh_mesh_source,
            const IStaticMesh::PositionSource& static_mesh_position_source,
            const ISector::Source& sector_source,
            uint32_t sector_base_index,
            const Activity& activity);
        std::vector<std::weak_ptr<IStaticMesh>> static_meshes() const override;
        void update(float delta) override;
        uint16_t water_scheme() const override;
        int32_t filterable_index() const override;
    private:
        void generate_geometry(const IMesh::Source& mesh_source, const trlevel::tr3_room& room);
        void generate_adjacency();
        void generate_static_meshes(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage,
            const IStaticMesh::MeshSource& static_mesh_mesh_source, const IStaticMesh::PositionSource& static_mesh_position_source, const Activity& activity);
        void render_contained(const ICamera& camera, const DirectX::SimpleMath::Color& colour, RenderFilter render_filter);
        void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour, RenderFilter render_filter);
        void generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ISector::Source& sector_source, uint32_t sector_base_index);
        ISector* get_trigger_sector(const ITrigger& trigger, int32_t dx, int32_t dz);
        uint32_t get_sector_id(int32_t x, int32_t z) const;

        /// Find any transparent triangles that match floor data geometry.
        /// @param transparent_triangles The transparent triangles in the sector.
        /// @param collision_triangles The collision output vector.
        void process_collision_transparency(std::vector<Triangle>& triangles);

        void generate_all_geometry_mesh(const IMesh::Source& mesh_source);

        void add_centroid_to_pick(const IMesh& mesh, PickResult& geometry_result) const;

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;
        uint32_t _index;

        std::vector<std::shared_ptr<IStaticMesh>> _static_meshes;

        std::shared_ptr<IMesh> _mesh;
        std::unordered_map<uint32_t, std::shared_ptr<IMesh>> _all_geometry_meshes;
        DirectX::SimpleMath::Matrix _room_offset;
        DirectX::SimpleMath::Matrix _inverted_room_offset;

        DirectX::BoundingBox  _bounding_box;

        std::vector<std::weak_ptr<IItem>> _entities;

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
        std::shared_ptr<ILevelTextureStorage> _texture_storage;
        std::vector<std::weak_ptr<ILight>> _lights;
        IMesh::Source _mesh_source;
        std::vector<uint32_t> _all_geometry_sector_rooms;

        bool _visible{ true };

        std::vector<std::weak_ptr<ICameraSink>> _camera_sinks;
        std::weak_ptr<ILevel> _level;
        Colour _ambient{ 1.0f, 1.0f, 1.0f };
        int16_t _ambient_intensity_1;
        int16_t _ambient_intensity_2;
        int16_t _light_mode;

        std::shared_ptr<graphics::ISamplerState> _geometry_sampler_state;
        std::shared_ptr<graphics::ISamplerState> _sampler_state;

        uint16_t _water_scheme{ 0u };
    };
}