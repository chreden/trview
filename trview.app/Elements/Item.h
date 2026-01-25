#pragma once

#include <cstdint>
#include <SimpleMath.h>

#include <memory>
#include <vector>

#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/IMesh.h>
#include "IItem.h"
#include "TypeInfo.h"

namespace trlevel
{
    struct ILevel;
    struct tr2_entity;
    struct tr_model;
    struct tr_sprite_sequence;
}

namespace trview
{
    struct ICamera;
    struct IModel;

    class Item final : public IItem, public std::enable_shared_from_this<IItem>
    {
    public:
        explicit Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const IModelStorage& model_storage, const std::weak_ptr<ILevel>& owning_level, uint32_t number, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, const std::weak_ptr<IRoom>& room);
        explicit Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr4_ai_object& entity, const IModelStorage& model_storage, const std::weak_ptr<ILevel>& owning_level, uint32_t number, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, const std::weak_ptr<IRoom>& room);
        virtual ~Item() = default;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        std::weak_ptr<IRoom> room() const override;
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
        std::weak_ptr<ILevel> level() const override;
        int32_t angle() const override;
        std::unordered_set<std::string> categories() const override;
        void set_categories(const std::unordered_set<std::string>& categories) override;
        void set_ng_plus(bool value) override;
        std::optional<bool> ng_plus() const override;
        bool is_ai() const override;
        void set_remastered_extra(bool value) override;
        bool is_remastered_extra() const override;
        int32_t filterable_index() const override;
    private:
        Item(const IMesh::Source& mesh_source, const IModelStorage& model_storage, const trlevel::ILevel& level, const std::weak_ptr<ILevel>& owning_level, const std::weak_ptr<IRoom>& room, uint32_t number, uint16_t type_id, const DirectX::SimpleMath::Vector3& position, int32_t angle, int32_t ocb, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, uint16_t flags);

        void generate_bounding_box();
        void apply_ocb_adjustment(trlevel::LevelVersion version, uint32_t ocb, bool is_pickup);
        bool is_pickup() const;

        DirectX::SimpleMath::Matrix               _world;
        std::shared_ptr<IMesh>                    _sprite_mesh;
        std::weak_ptr<IModel>                     _model;

        std::weak_ptr<IRoom>                      _room;
        uint32_t                                  _number;

        // Bits for sprites.
        DirectX::SimpleMath::Matrix               _scale;
        DirectX::SimpleMath::Matrix               _offset;
        DirectX::SimpleMath::Vector3              _position;

        DirectX::BoundingBox                      _bounding_box;
        bool                                      _visible{ true };
        bool _needs_ocb_adjustment{ false };
        TypeInfo _type;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        uint32_t _type_id{ 0u };
        int32_t _ocb{ 0u };
        uint16_t _flags{ 0u };

        std::weak_ptr<ILevel> _level;
        int32_t _angle;
        std::optional<bool> _ng_plus;
        bool _is_ai{ false };
        bool _is_remastered_extra{ false };
    };
}
