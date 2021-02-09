#pragma once

#include <cstdint>
#include <SimpleMath.h>

#include <memory>
#include <vector>

#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Geometry/IRenderable.h>

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
    class Mesh;
    struct ICamera;
    class TransparencyBuffer;

    class Entity : public IRenderable
    {
    public:
        explicit Entity(const graphics::Device& device, const trlevel::ILevel& level, const trlevel::tr2_entity& room, const ILevelTextureStorage& texture_storage, const IMeshStorage& mesh_storage, uint32_t index);
        virtual ~Entity() = default;
        virtual void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        uint16_t room() const;
        uint32_t index() const;

        virtual void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;

        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;
        DirectX::BoundingBox bounding_box() const;

        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
    private:
        void load_meshes(const trlevel::ILevel& level, int16_t type_id, const IMeshStorage& mesh_storage);
        void load_model(const trlevel::tr_model& model, const trlevel::ILevel& level);
        void load_sprite(const graphics::Device& device, const trlevel::tr_sprite_sequence& sprite_sequence, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage);
        void generate_bounding_box();
        void apply_ocb_adjustment(uint32_t ocb);

        DirectX::SimpleMath::Matrix               _world;
        std::vector<Mesh*>                        _meshes;
        std::unique_ptr<Mesh>                     _sprite_mesh;
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
    };
}
