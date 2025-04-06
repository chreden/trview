#include "Item.h"

#include <trview.common/Algorithms.h>
#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>

#include "../Graphics/ILevelTextureStorage.h"
#include "../Graphics/IMeshStorage.h"
#include "../Camera/ICamera.h"
#include "../Geometry/Matrix.h"
#include "../Geometry/IMesh.h"
#include "../Geometry/TransparencyBuffer.h"
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

    Item::Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const IMeshStorage& mesh_storage, const std::weak_ptr<ILevel>& owning_level, uint32_t number, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, const std::weak_ptr<IRoom>& room)
        : Item(mesh_source, mesh_storage, level, owning_level, room, number, entity.TypeID, entity.position(), entity.Angle, level.get_version() >= trlevel::LevelVersion::Tomb4 ? entity.Intensity2 : 0, type, triggers, entity.Flags)
    {
        
    }

    Item::Item(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr4_ai_object& entity, const IMeshStorage& mesh_storage, const std::weak_ptr<ILevel>& owning_level, uint32_t number, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, const std::weak_ptr<IRoom>& room)
        : Item(mesh_source, mesh_storage, level, owning_level, room, number, entity.type_id, entity.position(), entity.angle, entity.ocb, type, triggers, entity.flags)
    {
    }

    Item::Item(const IMesh::Source& mesh_source, const IMeshStorage& mesh_storage, const trlevel::ILevel& level, const std::weak_ptr<ILevel>& owning_level, const std::weak_ptr<IRoom>& room, uint32_t number, uint16_t type_id,
        const Vector3& position, int32_t angle, int32_t ocb, const TypeInfo& type, const std::vector<std::weak_ptr<ITrigger>>& triggers, uint16_t flags)
        : _room(room), _number(number), _type(type), _triggers(triggers), _type_id(type_id), _ocb(ocb), _flags(flags), _level(owning_level), _angle(angle)
    {
        // Extract the meshes required from the model.
        load_meshes(level, type_id, mesh_storage);

        trlevel::tr_model model;
        trlevel::tr_sprite_sequence sprite;

        if (level.get_model_by_id(type_id, model))
        {
            // Set up world matrix.
            _world = Matrix::CreateRotationY((angle / 16384.0f) * XM_PIDIV2) *
                Matrix::CreateTranslation(position);
            load_model(model, level);
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

    void Item::load_meshes(const trlevel::ILevel& level, int16_t type_id, const IMeshStorage& mesh_storage)
    {
        trlevel::tr_model model;
        if (level.get_model_by_id(level.get_mesh_from_type_id(type_id), model))
        {
            if (level.platform_and_version().platform == trlevel::Platform::PSX && 
                equals_any(level.platform_and_version().version, trlevel::LevelVersion::Tomb4, trlevel::LevelVersion::Tomb5))
            {
                const uint32_t end_pointer = static_cast<uint32_t>(model.StartingMesh + model.NumMeshes * 2);
                for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < end_pointer; mesh_pointer += 2)
                {
                    auto mesh = mesh_storage.mesh(mesh_pointer);
                    if (mesh)
                    {
                        _meshes.push_back(mesh);
                    }
                }
            }
            else
            {
                const uint32_t end_pointer = static_cast<uint32_t>(model.StartingMesh + model.NumMeshes);
                for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < end_pointer; ++mesh_pointer)
                {
                    auto mesh = mesh_storage.mesh(mesh_pointer);
                    if (mesh)
                    {
                        _meshes.push_back(mesh);
                    }
                }
            }
        }
    }

    void Item::load_model(const trlevel::tr_model& model, const trlevel::ILevel& level)
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        auto get_rotate = [](const trlevel::tr2_frame_rotation& r)
        {
            return Matrix::CreateFromYawPitchRoll(r.y, r.x, r.z);
        };

        if (model.NumMeshes > 0)
        {
            // Load the frames.
            auto frame = level.get_frame(trlevel::is_tr1_may_1996(level.platform_and_version()) ? model.FrameOffset : model.FrameOffset / 2, model.NumMeshes);

            uint32_t frame_offset = 0;

            auto initial_frame = frame.values[frame_offset++];

            Matrix initial_rotation = get_rotate(initial_frame);
            Matrix initial_frame_offset = Matrix::CreateTranslation(frame.position());

            Matrix previous_world = initial_rotation * initial_frame_offset;
            _world_transforms.push_back(previous_world);

            std::stack<Matrix> world_stack;

            // Build the mesh tree.
            // Request one less node than we have meshes as the first mesh is at the same position as the entity.
            auto mesh_nodes = level.get_meshtree(model.MeshTree, model.NumMeshes - 1);

            for (const auto& node : mesh_nodes)
            {
                Matrix parent_world = previous_world;

                if (node.Flags & 0x1)
                {
                    if (!world_stack.empty())
                    {
                        parent_world = world_stack.top();
                        world_stack.pop();
                    }
                    else
                    {
                        parent_world = Matrix::Identity;
                    }
                }
                if (node.Flags & 0x2)
                {
                    world_stack.push(parent_world);
                }

                // Get the rotation from the frames.
                // Rotations are performed in Y, X, Z order.
                auto rotation = frame.values[frame_offset++];
                Matrix rotation_matrix = get_rotate(rotation);
                Matrix translation_matrix = Matrix::CreateTranslation(node.position());
                Matrix node_transform = rotation_matrix * translation_matrix * parent_world;

                _world_transforms.push_back(node_transform);
                previous_world = node_transform;
            }
        }
    }

    void Item::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)
    {
        if (!_visible)
        {
            return;
        }

        using namespace DirectX::SimpleMath;

        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            auto wvp = _world_transforms[i] * _world * camera.view_projection();
            _meshes[i]->render(wvp, texture_storage, colour);
        }

        if (_sprite_mesh)
        {
            auto wvp = create_billboard(_position, _offset, _scale, camera) * camera.view_projection();
            _sprite_mesh->render(wvp, texture_storage, colour);
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

        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            for (const auto& triangle : _meshes[i]->transparent_triangles())
            {
                transparency.add(triangle.transform(_world_transforms[i] * _world, colour, true));
            }
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
        if (!_bounding_box.Intersects(position, direction, box_distance))
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

        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        std::vector<uint32_t> pick_meshes;

        // Check each of the meshes in the object.
        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            // Try and pick against the bounding box.
            float obb_distance = 0;
            if (_oriented_boxes[i].Intersects(position, direction, obb_distance))
            {
                // Pick against the triangles in this mesh.
                pick_meshes.push_back(i);
            }
        }

        if (pick_meshes.empty())
        {
            return PickResult();
        }

        PickResult result;
        result.type = PickResult::Type::Entity;
        result.item = std::const_pointer_cast<IItem>(shared_from_this());

        for (auto i : pick_meshes)
        {
            // Transform the position and the direction into mesh space.
            auto transform = (_world_transforms[i] * _world).Invert();
            auto transformed_position = Vector3::Transform(position, transform);
            auto transformed_direction = Vector3::TransformNormal(direction, transform);

            // Pick against mesh.
            auto mesh_result = _meshes[i]->pick(transformed_position, transformed_direction);
            if (mesh_result.hit && mesh_result.distance < result.distance)
            {
                result.hit = true;
                result.distance = mesh_result.distance;
                result.position = position + direction * mesh_result.distance;
            }
        }

        return result;
    }

    void Item::generate_bounding_box()
    {
        using namespace DirectX;

        // Sprite meshes not yet handled.
        if (_meshes.empty())
        {
            if (_sprite_mesh)
            {
                float width = _scale._11;
                float height = _scale._22;
                BoundingSphere sphere(_position, sqrt(width * width + height * height) * 0.5f);
                sphere.Transform(sphere, _offset);
                BoundingBox::CreateFromSphere(_bounding_box, sphere);
            }
            return;
        }

        using namespace DirectX::SimpleMath;

        // Remove any stored boxes so they can be created.
        _oriented_boxes.clear();

        // The entity bounding box is based on the bounding boxes of the meshes it contains.
        // Allocate space for all of the corners.
        std::vector<Vector3> corners(_meshes.size() * 8);

        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            // Get the box for the mesh.
            auto box = _meshes[i]->bounding_box();

            // Transform the box by the model transform. Store this box for later picking.
            BoundingOrientedBox oriented_box;
            BoundingOrientedBox::CreateFromBoundingBox(oriented_box, box);
            oriented_box.Transform(oriented_box, _world_transforms[i] * _world);
            oriented_box.GetCorners(&corners[i * 8]);
            _oriented_boxes.push_back(oriented_box);
        }

        // Create an axis-aligned BB from the points of the oriented ones.
        BoundingBox::CreateFromPoints(_bounding_box, corners.size(), &corners[0], sizeof(Vector3));
    }

    void Item::apply_ocb_adjustment(trlevel::LevelVersion version, uint32_t ocb, bool is_pickup)
    {
        if (!is_pickup || !needs_adjustment(version, ocb))
        {
            return;
        }

        using namespace DirectX::SimpleMath;
        Matrix offset = Matrix::CreateTranslation(0, -_bounding_box.Extents.y, 0);
        _world *= offset;

        for (auto& obb : _oriented_boxes)
        {
            obb.Transform(obb, offset);
        }
        _bounding_box.Transform(_bounding_box, offset);
        _needs_ocb_adjustment = true;
    }

    DirectX::BoundingBox Item::bounding_box() const
    {
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

        for (auto& obb : _oriented_boxes)
        {
            obb.Transform(obb, offset);
        }
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
