#pragma once

#include <cstdint>
#include <SimpleMath.h>

#include <memory>
#include <vector>

#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/IMesh.h>
#include "IItem.h"

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

    class Item final : public IItem
    {
    public:
        explicit Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const IMeshStorage& mesh_storage, uint32_t number, const std::string& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, bool is_pickup);
        explicit Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr4_ai_object& entity, const IMeshStorage& mesh_storage, uint32_t number, const std::string& type, const std::vector<std::weak_ptr<ITrigger>>& triggers);
        virtual ~Item() = default;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual uint16_t room() const override;
        virtual uint32_t number() const override;

        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual DirectX::BoundingBox bounding_box() const override;

        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
        virtual void adjust_y(float amount) override;
        virtual bool needs_ocb_adjustment() const override;
        std::string type() const override;
        std::vector<std::weak_ptr<ITrigger>> triggers() const override;
        uint32_t type_id() const override;
        int32_t ocb() const override;
        uint16_t activation_flags() const override;
        bool clear_body_flag() const override;
        bool invisible_flag() const override;
        DirectX::SimpleMath::Vector3 position() const override;
    private:
        Item(const IMesh::Source& mesh_source, const IMeshStorage& mesh_storage, const trlevel::ILevel& level, uint16_t room, uint32_t number, uint16_t type_id, const DirectX::SimpleMath::Vector3& position, int32_t angle, int32_t ocb, const std::string& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, uint16_t flags, bool is_pickup);

        void load_meshes(const trlevel::ILevel& level, int16_t type_id, const IMeshStorage& mesh_storage);
        void load_model(const trlevel::tr_model& model, const trlevel::ILevel& level);
        void generate_bounding_box();
        void apply_ocb_adjustment(trlevel::LevelVersion version, uint32_t ocb, bool is_pickup);

        DirectX::SimpleMath::Matrix               _world;
        std::vector<std::shared_ptr<IMesh>>       _meshes;
        std::shared_ptr<IMesh>                    _sprite_mesh;
        std::vector<DirectX::SimpleMath::Matrix>  _world_transforms;
        uint16_t                                  _room;
        uint32_t                                  _number;

        // Bits for sprites.
        DirectX::SimpleMath::Matrix               _scale;
        DirectX::SimpleMath::Matrix               _offset;
        DirectX::SimpleMath::Vector3              _position;

        DirectX::BoundingBox                      _bounding_box;
        std::vector<DirectX::BoundingOrientedBox> _oriented_boxes;
        bool                                      _visible{ true };
        bool _needs_ocb_adjustment{ false };

        std::string _type;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        uint32_t _type_id{ 0u };
        int32_t _ocb{ 0u };
        uint16_t _flags{ 0u };
    };
}
