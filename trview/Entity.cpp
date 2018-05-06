#include "stdafx.h"
#include "Entity.h"

#include <stack>
#include <algorithm>

#include "ILevelTextureStorage.h"
#include "IMeshStorage.h"
#include "ICamera.h"
#include "Mesh.h"

#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>

namespace trview
{
    Entity::Entity(CComPtr<ID3D11Device> device, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const ILevelTextureStorage& texture_storage, const IMeshStorage& mesh_storage)
        : _device(device), _room(entity.Room)
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        // Extract the meshes required from the model.
        trlevel::tr_model model;
        trlevel::tr_sprite_sequence sprite;

        if (level.get_model_by_id(entity.TypeID, model))
        {
            // Set up world matrix.
            _world = Matrix::CreateRotationY((entity.Angle / 16384.0f) * XM_PIDIV2) * 
                     Matrix::CreateTranslation(entity.x / 1024.0f, entity.y / -1024.0f, entity.z / 1024.0f);
            load_model(model, level, mesh_storage);
        }
        else if (level.get_sprite_sequence_by_id(entity.TypeID, sprite))
        {
            _world = Matrix::CreateTranslation(entity.x / 1024.0f, entity.y / -1024.0f, entity.z / 1024.0f);
            load_sprite(sprite, level, texture_storage);
            _position = Vector3(entity.x / 1024.0f, entity.y / -1024.0f, entity.z / 1024.0f);
        }
    }

    void Entity::load_model(const trlevel::tr_model& model, const trlevel::ILevel& level, const IMeshStorage& mesh_storage)
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        const uint32_t end_pointer = static_cast<uint32_t>(model.StartingMesh + model.NumMeshes);
        for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < end_pointer; ++mesh_pointer)
        {
            _meshes.push_back(mesh_storage.mesh(mesh_pointer));
        }

        if (model.NumMeshes > 0)
        {
            // Load the frames.
            auto frame = level.get_frame(model.FrameOffset / 2, model.NumMeshes);

            uint32_t frame_offset = 0;

            auto initial_frame = frame.values[frame_offset++];

            Matrix initial_rotation = Matrix::CreateFromYawPitchRoll(initial_frame.y, XM_2PI - initial_frame.x, XM_2PI - initial_frame.z);
            Matrix initial_frame_offset = Matrix::CreateTranslation(frame.offsetx / 1024.0f, frame.offsety / -1024.0f, frame.offsetz / 1024.0f);

            _world = initial_rotation * initial_frame_offset * _world;

            Matrix previous_world = Matrix::Identity;
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
                Matrix rotation_matrix = Matrix::CreateFromYawPitchRoll(rotation.y, XM_2PI - rotation.x, XM_2PI - rotation.z);
                Matrix translation_matrix = Matrix::CreateTranslation(node.Offset_X / 1024.0f, node.Offset_Y / -1024.0f, node.Offset_Z / 1024.0f);
                Matrix node_transform = rotation_matrix * translation_matrix * parent_world;

                _world_transforms.push_back(node_transform);
                previous_world = node_transform;
            }
        }
    }

    void Entity::load_sprite(const trlevel::tr_sprite_sequence& sprite_sequence, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage)
    {
        // Get the first sprite image.
        auto sprite = level.get_sprite_texture(sprite_sequence.Offset);
        auto texture = texture_storage.texture(sprite.Tile);

        // Calculate UVs.
        float u = static_cast<float>(sprite.x) / 256.0f;
        float v = static_cast<float>(sprite.y) / 256.0f;
        float width = static_cast<float>((sprite.Width - 255) / 256) / 256.0f;
        float height = static_cast<float>((sprite.Height - 255) / 256) / 256.0f;

        // Generate quad.
        using namespace DirectX::SimpleMath;
        std::vector<MeshVertex> vertices
        {
            { Vector3(-0.5f, 0.5f, 0), Vector2(u, v), Vector4(1,1,1,1) },
            { Vector3(0.5f, 0.5f, 0), Vector2(u + width, v), Vector4(1,1,1,1) },
            { Vector3(-0.5f, -0.5f, 0), Vector2(u, v + height), Vector4(1,1,1,1) },
            { Vector3(0.5f, -0.5f, 0), Vector2(u + width, v + height), Vector4(1,1,1,1) },
        };

        std::vector<TransparentTriangle> transparent_triangles
        {
            { vertices[0].pos, vertices[1].pos, vertices[2].pos, vertices[0].uv, vertices[1].uv, vertices[2].uv, sprite.Tile, TransparentTriangle::Mode::Normal },
            { vertices[2].pos, vertices[1].pos, vertices[3].pos, vertices[2].uv, vertices[1].uv, vertices[3].uv, sprite.Tile, TransparentTriangle::Mode::Normal },
        };

        _sprite_mesh = std::make_unique<Mesh>(_device, std::vector<MeshVertex>(), std::vector<std::vector<uint32_t>>(), std::vector<uint32_t>(), transparent_triangles);

        // Scale is computed from the 'side' values.
        float object_width = static_cast<float>(sprite.RightSide - sprite.LeftSide) / 1024.0f;
        float object_height = static_cast<float>(sprite.BottomSide - sprite.TopSide) / 1024.0f;
        _scale = Matrix::CreateScale(object_width, object_height, 1);

        // An offset to move the sprite up a bit.
        _offset = Matrix::CreateTranslation(0, object_height / 2.0f, 0);
    }

    void Entity::render(CComPtr<ID3D11DeviceContext> context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)
    {
        using namespace DirectX::SimpleMath;

        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            auto wvp = _world_transforms[i] * _world * camera.view_projection();
            _meshes[i]->render(context, wvp, texture_storage, colour);
        }

        if (_sprite_mesh)
        {
            Vector3 forward = camera.forward();
            auto billboard = Matrix::CreateBillboard(_position, camera.position(), camera.up(), &forward);
            auto world = _scale * billboard * _offset;
            auto wvp = world * camera.view_projection();
            _sprite_mesh->render(context, wvp, texture_storage, colour);
        }
    }

    uint16_t Entity::room() const
    {
        return _room;
    }

    void Entity::get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            for (const auto& triangle : _meshes[i]->transparent_triangles())
            {
                transparency.add(triangle.transform(_world_transforms[i] * _world, colour));
            }
        }

        if (_sprite_mesh)
        {
            for (const auto& triangle : _sprite_mesh->transparent_triangles())
            {
                using namespace DirectX::SimpleMath;
                Vector3 forward = camera.forward();
                auto billboard = Matrix::CreateBillboard(_position, camera.position(), camera.up(), &forward);
                auto world = _scale * billboard * _offset;
                transparency.add(triangle.transform(world, colour));
            }
        }
    }
}
