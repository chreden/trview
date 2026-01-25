#include "Item.h"

#include <trview.common/Algorithms.h>
#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>

#include "../Camera/ICamera.h"
#include "../Geometry/Matrix.h"
#include "../Geometry/IMesh.h"
#include "../Geometry/TransparencyBuffer.h"
#include "../Geometry/Model/IModel.h"
#include "../Geometry/Model/IModelStorage.h"
#include "IRoom.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        /// <summary>
        /// Determines whether the TR4 object type needs any adjustment to its position based on the ocb value.
        /// </summary>
        /// <param name="ocb">The ocb value.</param>
        /// <returns>Whether the position needs adjustment.</returns>
        bool needs_adjustment_tr4(uint32_t ocb)
        {
            return equals_any(ocb & 0x3f, 0u, 3u, 4u);
        }

        /// <summary>
        /// Determines whether the TR5 object type needs any adjustment to its position based on the ocb value.
        /// </summary>
        /// <param name="ocb">The ocb value.</param>
        /// <returns>Whether the position needs adjustment.</returns>
        bool needs_adjustment_tr5(uint32_t ocb)
        {
            return equals_any(ocb & 0x3f, 0u, 3u, 4u, 5u, 6u, 7u, 8u, 11u);
        }

        /// <summary>
        /// Determines whether the object needs position adjustment.
        /// </summary>
        /// <param name="version">The version of the level.</param>
        /// <param name="ocb">OCB value of the object.</param>
        /// <returns>Whether the object position needs to be adjusted.</returns>
        bool needs_adjustment(trlevel::LevelVersion version, uint32_t ocb)
        {
            if (version == trlevel::LevelVersion::Tomb4)
            {
                return needs_adjustment_tr4(ocb);
            }
            else if (version == trlevel::LevelVersion::Tomb5)
            {
                return needs_adjustment_tr5(ocb);
            }
            return false;
        }
    }

    IItem::~IItem()
    {
    }

    Item::Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const IModelStorage& model_storage, const std::weak_ptr<ILevel>& owning_level, uint32_t number, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, const std::weak_ptr<IRoom>& room)
        : Item(mesh_source, model_storage, level, owning_level, room, number, entity.TypeID, entity.position(), entity.Angle, level.get_version() >= trlevel::LevelVersion::Tomb4 ? entity.Intensity2 : 0, type, triggers, entity.Flags)
    {
        
    }

    Item::Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr4_ai_object& entity, const IModelStorage& model_storage, const std::weak_ptr<ILevel>& owning_level, uint32_t number, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, const std::weak_ptr<IRoom>& room)
        : Item(mesh_source, model_storage, level, owning_level, room, number, entity.type_id, entity.position(), entity.angle, entity.ocb, type, triggers, entity.flags)
    {
        _is_ai = true;
    }

    Item::Item(const IMesh::Source& mesh_source, const IModelStorage& model_storage, const trlevel::ILevel& level, const std::weak_ptr<ILevel>& owning_level, const std::weak_ptr<IRoom>& room, uint32_t number, uint16_t type_id,
        const Vector3& position, int32_t angle, int32_t ocb, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, uint16_t flags)
        : _room(room), _number(number), _type(type), _triggers(triggers), _type_id(type_id), _ocb(ocb), _flags(flags), _level(owning_level), _angle(angle)
    {
        _model = model_storage.find_by_type_id(type_id);

        trlevel::tr_sprite_sequence sprite;
        
        if (_model.lock())
        {
            // Set up world matrix.
            _world = Matrix::CreateRotationY((angle / 16384.0f) * XM_PIDIV2) *
                Matrix::CreateTranslation(position);
        }
        else if (level.get_sprite_sequence_by_id(type_id, sprite))
        {
            _world = Matrix::CreateTranslation(position);
            _sprite_mesh = create_sprite_mesh(mesh_source, level.get_sprite_texture(sprite.Offset), _scale, _offset, SpriteOffsetMode::Entity);
        }

        _position = position;

        generate_bounding_box();
        apply_ocb_adjustment(level.get_version(), ocb, is_pickup());
    }

    void Item::render(const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (!_visible)
        {
            return;
        }

        using namespace DirectX::SimpleMath;

        if (auto model = _model.lock())
        {
            model->render(_world, camera.view_projection(), colour);
        }

        if (_sprite_mesh)
        {
            auto wvp = create_billboard(_position, _offset, _scale, camera) * camera.view_projection();
            _sprite_mesh->render(wvp, colour);
        }
    }

    std::weak_ptr<IRoom> Item::room() const
    {
        return _room;
    }

    uint32_t Item::number() const
    {
        return _number;
    }

    void Item::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (!_visible)
        {
            return;
        }

        if (auto model = _model.lock())
        {
            model->render_transparency(_world, transparency, colour);
        }

        if (_sprite_mesh)
        {
            auto world = create_billboard(_position, _offset, _scale, camera);
            for (const auto& triangle : _sprite_mesh->transparent_triangles())
            {
                transparency.add(triangle.transform(world, colour, true));
            }
        }
    }

    PickResult Item::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        // Check against some sort of bounding box (based on the mesh?)
        // Test against bounding box for the room first, to avoid more expensive mesh-ray intersection
        float box_distance = 0;
        if (!bounding_box().Intersects(position, direction, box_distance))
        {
            return PickResult();
        }

        if (_sprite_mesh)
        {
            PickResult result;
            result.hit = true;
            result.type = PickResult::Type::Entity;
            result.item = std::const_pointer_cast<IItem>(shared_from_this());
            result.distance = box_distance;
            result.position = position + direction * box_distance;
            result.item = std::const_pointer_cast<IItem>(shared_from_this());
            return result;
        }

        if (auto model = _model.lock())
        {
            auto result = model->pick(_world, position, direction);
            if (result.hit)
            {
                result.item = std::const_pointer_cast<IItem>(shared_from_this());
                result.type = PickResult::Type::Entity;
                return result;
            }
        }
        return {};
    }

    void Item::generate_bounding_box()
    {
        using namespace DirectX;

        // Sprite meshes not yet handled.
        const auto model = _model.lock();
        if (!model && _sprite_mesh)
        {
            float width = _scale._11;
            float height = _scale._22;
            BoundingSphere sphere(_position, sqrt(width * width + height * height) * 0.5f);
            sphere.Transform(sphere, _offset);
            BoundingBox::CreateFromSphere(_bounding_box, sphere);
        }
    }

    void Item::apply_ocb_adjustment(trlevel::LevelVersion version, uint32_t ocb, bool is_pickup)
    {
        if (!is_pickup || !needs_adjustment(version, ocb))
        {
            return;
        }

        float height = _bounding_box.Extents.y;
        if (auto model = _model.lock())
        {
            height = model->bounding_box().Extents.y;
        }

        using namespace DirectX::SimpleMath;
        Matrix offset = Matrix::CreateTranslation(0, -height, 0);
        _world *= offset;
        _bounding_box.Transform(_bounding_box, offset);
        _needs_ocb_adjustment = true;
    }

    DirectX::BoundingBox Item::bounding_box() const
    {
        if (auto model = _model.lock())
        {
            BoundingBox transformed_box;
            model->bounding_box().Transform(transformed_box, _world);
            return transformed_box;
        }
        return _bounding_box;
    }

    bool Item::visible() const
    {
        return _visible;
    }

    void Item::set_visible(bool value)
    {
        _visible = value;
        on_changed();
    }

    void Item::adjust_y(float amount)
    {
        auto offset = Matrix::CreateTranslation(0, amount, 0);
        _world *= offset;
        _bounding_box.Transform(_bounding_box, offset);
    }

    bool Item::needs_ocb_adjustment() const
    {
        return _needs_ocb_adjustment;
    }

    std::string Item::type() const
    {
        return _type.name;
    }

    std::vector<std::weak_ptr<ITrigger>> Item::triggers() const
    {
        return _triggers;
    }

    uint32_t Item::type_id() const
    {
        return _type_id;
    }

    int32_t Item::ocb() const
    {
        return _ocb;
    }

    uint16_t Item::activation_flags() const
    {
        return (_flags & 0x3E00) >> 9;
    }

    bool Item::clear_body_flag() const
    {
        return (_flags & 0x8000) != 0;
    }

    bool Item::invisible_flag() const
    {
        return (_flags & 0x100) != 0;
    }

    Vector3 Item::position() const
    {
        return _position;
    }

    std::weak_ptr<ILevel> Item::level() const
    {
        return _level;
    }

    int32_t Item::angle() const
    {
        return _angle;
    }

    bool Item::is_pickup() const
    {
        return _type.categories.find("Pickup") != _type.categories.end();
    }

    std::unordered_set<std::string> Item::categories() const
    {
        return _type.categories;
    }

    void Item::set_categories(const std::unordered_set<std::string>& categories)
    {
        _type.categories = categories;
    }

    void Item::set_ng_plus(bool value)
    {
        _ng_plus = value;
    }

    std::optional<bool> Item::ng_plus() const
    {
        return _ng_plus;
    }

    bool Item::is_ai() const
    {
        return _is_ai;
    }

    void Item::set_remastered_extra(bool value)
    {
        _is_remastered_extra = value;
    }

    bool Item::is_remastered_extra() const
    {
        return _is_remastered_extra;
    }

    int32_t Item::filterable_index() const
    {
        return static_cast<int32_t>(number());
    }

    bool is_mutant_egg(const IItem& item)
    {
        return is_mutant_egg(item.type_id());
    }

    bool is_mutant_egg(uint32_t type_id)
    {
        return equals_any(type_id, 163u, 181u);
    }

    uint16_t mutant_egg_contents(const IItem& item)
    {
        return mutant_egg_contents(item.activation_flags());
    }

    uint16_t mutant_egg_contents(uint16_t flags)
    {
        switch (flags)
        {
        case 1:
            return 21; // Shooter
        case 2:
            return 23; // Centaur
        case 4:
            return 34; // Torso
        case 8:
            return 22; // Mutant
        }
        return 20; // Winged
    }

    uint32_t item_room(const std::shared_ptr<IItem>& item)
    {
        if (!item)
        {
            return 0u;
        }
        return item_room(*item);
    }

    uint32_t item_room(const IItem& item)
    {
        if (auto room = item.room().lock())
        {
            return room->number();
        }
        return 0u;
    }

    bool is_virtual(const IItem& item)
    {
        const auto categories = item.categories();
        return categories.find("Virtual") != categories.end();
    }
}
