#include "Entity.h"

#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Graphics/IMeshStorage.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Geometry/Matrix.h>
#include <trview.app/Geometry/IMesh.h>
#include <trview.app/Geometry/TransparencyBuffer.h>
#include <trview.common/Algorithms.h>
#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    Entity::Entity(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const IMeshStorage& mesh_storage, uint32_t index)
        : Entity(mesh_source, mesh_storage, level, entity.Room, index, entity.TypeID, entity.position(), entity.Angle, entity.Intensity2)
    {
        
    }

    Entity::Entity(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr4_ai_object& entity, const IMeshStorage& mesh_storage, uint32_t index)
        : Entity(mesh_source, mesh_storage, level, entity.room, index, entity.type_id, entity.position(), entity.angle, entity.ocb)
    {
    }

    Entity::Entity(const IMesh::Source& mesh_source, const IMeshStorage& mesh_storage, const trlevel::ILevel& level, uint32_t room, uint32_t index, uint32_t type_id,
        const Vector3& position, int32_t angle, int32_t ocb)
        : _room(room), _index(index)
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
            _position = position;
        }

        generate_bounding_box();

        if (level.get_version() >= trlevel::LevelVersion::Tomb4)
        {
            apply_ocb_adjustment(ocb);
        }
    }

    void Entity::load_meshes(const trlevel::ILevel& level, int16_t type_id, const IMeshStorage& mesh_storage)
    {
        trlevel::tr_model model;
        if (level.get_model_by_id(level.get_mesh_from_type_id(type_id), model))
        {
            const uint32_t end_pointer = static_cast<uint32_t>(model.StartingMesh + model.NumMeshes);
            for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < end_pointer; ++mesh_pointer)
            {
                _meshes.push_back(mesh_storage.mesh(mesh_pointer));
            }
        }
    }

    void Entity::load_model(const trlevel::tr_model& model, const trlevel::ILevel& level)
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
            auto frame = level.get_frame(model.FrameOffset / 2, model.NumMeshes);

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
                    parent_world = world_stack.top();
                    world_stack.pop();
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

    void Entity::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)
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

    uint16_t Entity::room() const
    {
        return _room;
    }

    uint32_t Entity::index() const
    {
        return _index;
    }

    void Entity::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (!_visible)
        {
            return;
        }

        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            for (const auto& triangle : _meshes[i]->transparent_triangles())
            {
                transparency.add(triangle.transform(_world_transforms[i] * _world, colour));
            }
        }

        if (_sprite_mesh)
        {
            auto world = create_billboard(_position, _offset, _scale, camera);
            for (const auto& triangle : _sprite_mesh->transparent_triangles())
            {
                transparency.add(triangle.transform(world, colour));
            }
        }
    }

    PickResult Entity::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
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
            result.index = _index;
            result.distance = box_distance;
            result.position = position + direction * box_distance;
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
        result.index = _index;

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

    void Entity::generate_bounding_box()
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

    void Entity::apply_ocb_adjustment(uint32_t ocb)
    {
        using namespace DirectX::SimpleMath;
        const int flags = ocb & 0x3F;
        // This assumes that pickups only have one mesh, as a general rule (it seems to work most of the time).
        // Applying OCB adjustment without this test when OCB is 0 makes Lara and other entities move up, which
        // isn't right.
        if (_meshes.size() == 1 && equals_any(flags, 0, 3, 7, 11))
        {
            Matrix offset = Matrix::CreateTranslation(0, -_bounding_box.Extents.y, 0);
            _world *= offset;

            for (auto& obb : _oriented_boxes)
            {
                obb.Transform(obb, offset);
            }
            _bounding_box.Transform(_bounding_box, offset);
        }
    }

    DirectX::BoundingBox Entity::bounding_box() const
    {
        return _bounding_box;
    }

    bool Entity::visible() const
    {
        return _visible;
    }

    void Entity::set_visible(bool value)
    {
        _visible = value;
    }
}
