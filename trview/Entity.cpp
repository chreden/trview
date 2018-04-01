#include "stdafx.h"
#include "Entity.h"

#include <stack>
#include <algorithm>

#include "ILevelTextureStorage.h"
#include "IMeshStorage.h"
#include "Mesh.h"

#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>

namespace trview
{
    Entity::Entity(CComPtr<ID3D11Device> device, const trlevel::ILevel& level, const trlevel::tr2_entity& entity, const ILevelTextureStorage& texture_storage, const IMeshStorage& mesh_storage)
        : _device(device), _room(entity.Room)
    {
        using namespace DirectX;

        // Set up world matrix.
        _world = XMMatrixMultiply(
            XMMatrixRotationY((entity.Angle / 16384.0f) * XM_PIDIV2),
            XMMatrixTranslation(entity.x / 1024.0f, entity.y / -1024.0f, entity.z / 1024.0f));

        // Extract the meshes required from the model.
        trlevel::tr_model model;
        trlevel::tr_sprite_sequence sprite;

        if (level.get_model_by_id(entity.TypeID, model))
        {
            load_model(model, level, mesh_storage);
        }
        else if (level.get_sprite_sequence_by_id(entity.TypeID, sprite))
        {
            load_sprite(sprite, level, texture_storage);
        }
    }

    void Entity::load_model(const trlevel::tr_model& model, const trlevel::ILevel& level, const IMeshStorage& mesh_storage)
    {
        using namespace DirectX;

        for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < model.StartingMesh + model.NumMeshes; ++mesh_pointer)
        {
            _meshes.push_back(mesh_storage.mesh(mesh_pointer));
        }

        if (model.NumMeshes > 0)
        {
            // Load the frames.
            auto frame = level.get_frame(model.FrameOffset / 2, model.NumMeshes);

            uint32_t frame_offset = 0;

            auto initial_frame = frame.values[frame_offset++];

            XMMATRIX initial_rotation = XMMatrixRotationRollPitchYaw(XM_2PI - initial_frame.x, initial_frame.y, XM_2PI - initial_frame.z);
            XMMATRIX initial_frame_offset = XMMatrixTranslation(frame.offsetx / 1024.0f, frame.offsety / -1024.0f, frame.offsetz / 1024.0f);

            _world = XMMatrixMultiply(XMMatrixMultiply(initial_rotation, initial_frame_offset), _world);

            XMMATRIX initial_world = XMMatrixIdentity();
            _world_transforms.push_back(initial_world);

            XMMATRIX previous_world = initial_world;

            std::stack<XMMATRIX> world_stack;

            // Build the mesh tree.
            // Request one less node than we have meshes as the first mesh is at the same position as the entity.
            auto mesh_nodes = level.get_meshtree(model.MeshTree, model.NumMeshes - 1);

            for (const auto& node : mesh_nodes)
            {
                XMMATRIX parent_world = previous_world;

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
                XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(XM_2PI - rotation.x, rotation.y, XM_2PI - rotation.z);
                XMMATRIX translation_matrix = XMMatrixTranslation(node.Offset_X / 1024.0f, node.Offset_Y / -1024.0f, node.Offset_Z / 1024.0f);
                XMMATRIX node_transform = XMMatrixMultiply(XMMatrixMultiply(rotation_matrix, translation_matrix), parent_world);

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
        using namespace DirectX;
        std::vector<MeshVertex> vertices
        {
            { XMFLOAT3(-0.5f, 0.5f, 0), XMFLOAT2(u, v), XMFLOAT4(1,1,1,1) },
            { XMFLOAT3(0.5f, 0.5f, 0), XMFLOAT2(u + width, v), XMFLOAT4(1,1,1,1) },
            { XMFLOAT3(-0.5f, -0.5f, 0), XMFLOAT2(u, v + height), XMFLOAT4(1,1,1,1) },
            { XMFLOAT3(0.5f, -0.5f, 0), XMFLOAT2(u + width, v + height), XMFLOAT4(1,1,1,1) },
        };

        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        indices[sprite.Tile] = { 0, 1, 2, 2, 1, 3 };

        _sprite_mesh = std::make_unique<Mesh>(
            _device,
            vertices,
            indices,
            std::vector<uint32_t>(),
            texture_storage);

        // Scale is computed from the 'side' values.

    }

    void Entity::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, const ILevelTextureStorage& texture_storage, const DirectX::XMFLOAT4& colour)
    {
        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            auto w = DirectX::XMMatrixMultiply(_world_transforms[i], _world);
            auto wvp = DirectX::XMMatrixMultiply(w, view_projection);
            _meshes[i]->render(context, wvp, texture_storage, colour);
        }

        if (_sprite_mesh)
        {
            auto wvp = DirectX::XMMatrixMultiply(_world, view_projection);
            _sprite_mesh->render(context, wvp, texture_storage, colour);
        }
    }

    uint16_t Entity::room() const
    {
        return _room;
    }
}
