#pragma once

#include <cstdint>
#include <SimpleMath.h>

#include <memory>
#include <vector>

#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/IMesh.h>
#include "IEntity.h"

namespace trlevel
{
    struct ILevel;
    struct tr2_entity;
    struct tr_model;
    struct tr_sprite_sequence;
}

namespace trview
{
    struct IMeshStorage;
    struct ILevelTextureStorage;
    struct ICamera;

    class Entity final : public IEntity
    {
    public:
        explicit Entity(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const IMeshStorage& mesh_storage, uint32_t index, bool is_pickup);
        explicit Entity(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr4_ai_object& entity, const IMeshStorage& mesh_storage, uint32_t index);
        virtual ~Entity() = default;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual uint16_t room() const override;
        uint32_t index() const;

        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual DirectX::BoundingBox bounding_box() const override;

        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
        virtual void adjust_y(float amount) override;
        virtual bool needs_ocb_adjustment() const override;
    private:
        Entity(const IMesh::Source& mesh_source, const IMeshStorage& mesh_storage, const trlevel::ILevel& level, uint32_t room, uint32_t index, uint32_t type_id, const DirectX::SimpleMath::Vector3& position, int32_t angle, int32_t ocb, bool is_pickup);

        void load_meshes(const trlevel::ILevel& level, int16_t type_id, const IMeshStorage& mesh_storage);
        void load_model(const trlevel::tr_model& model, const trlevel::ILevel& level);
        void generate_bounding_box();
        void apply_ocb_adjustment(trlevel::LevelVersion version, uint32_t ocb, bool is_pickup);

        DirectX::SimpleMath::Matrix               _world;
        std::vector<std::shared_ptr<IMesh>>       _meshes;
        std::shared_ptr<IMesh>                    _sprite_mesh;
        std::vector<DirectX::SimpleMath::Matrix>  _world_transforms;
        uint16_t                                  _room;
        uint32_t                                  _index;

        // Bits for sprites.
        DirectX::SimpleMath::Matrix               _scale;
        DirectX::SimpleMath::Matrix               _offset;
        DirectX::SimpleMath::Vector3              _position;

        DirectX::BoundingBox                      _bounding_box;
        std::vector<DirectX::BoundingOrientedBox> _oriented_boxes;
        bool                                      _visible{ true };
        bool _needs_ocb_adjustment{ false };
    };
}
