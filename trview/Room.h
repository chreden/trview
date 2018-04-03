#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>

#include <d3d11.h>
#include <atlbase.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "RoomInfo.h"
#include <trview.common/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "StaticMesh.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct IMeshStorage;
    class Entity;
    struct ICamera;

    class Room
    {
    public:
        enum class SelectionMode
        {
            Selected,
            NotSelected,
            Neighbour
        };

        struct PickResult
        {
            bool              hit{ false };
            float             distance;
            DirectX::XMVECTOR position;
        };

        explicit Room(CComPtr<ID3D11Device> device, 
            const trlevel::ILevel& level, 
            const trlevel::tr3_room& room,
            const ILevelTextureStorage& texture_storage,
            const IMeshStorage& mesh_storage);

        Room(const Room&) = delete;
        Room& operator=(const Room&) = delete;

        RoomInfo           info() const;
        std::set<uint16_t> neighbours() const;

        // Determine whether the specified ray hits any of the triangles in the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space.
        PickResult pick(DirectX::XMVECTOR position, DirectX::XMVECTOR direction) const;

        void render(CComPtr<ID3D11DeviceContext> context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected);

        // Add the specified entity to the room.
        // Entity: The entity to add.
        void add_entity(Entity* entity);
    private:
        void generate_geometry(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage);
        void generate_adjacency(const trlevel::ILevel& level, const trlevel::tr3_room& room);
        void generate_static_meshes(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage);

        struct Triangle
        {
            Triangle(DirectX::XMVECTOR v0, DirectX::XMVECTOR v1, DirectX::XMVECTOR v2)
                : v0(v0), v1(v1), v2(v2), normal(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v0), DirectX::XMVectorSubtract(v2, v0)))
            {
            }

            DirectX::XMVECTOR v0;
            DirectX::XMVECTOR v1;
            DirectX::XMVECTOR v2;
            DirectX::XMVECTOR normal;
        };

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;

        std::vector<std::unique_ptr<StaticMesh>> _static_meshes;

        // Rendering bits:
        CComPtr<ID3D11Device>              _device;
        CComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<CComPtr<ID3D11Buffer>> _index_buffers;
        std::vector<uint32_t>              _index_counts;
        CComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                           _untextured_index_count;
        CComPtr<ID3D11Buffer>              _matrix_buffer;
        DirectX::XMMATRIX                  _room_offset;

        // Triangle copy for ray intersection.
        std::vector<Triangle> _collision_triangles;
        DirectX::BoundingBox  _bounding_box;

        std::vector<Entity*> _entities;
    };
}