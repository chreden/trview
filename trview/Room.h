#pragma once

#include <vector>
#include <cstdint>
#include <set>
#include <memory>

#include <d3d11.h>
#include <atlbase.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>

#include "RoomInfo.h"
#include <trview.common/Texture.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "StaticMesh.h"
#include "TransparencyBuffer.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct IMeshStorage;
    class Entity;
    struct ICamera;
    class Mesh;
    class TransparencyBuffer;

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
            bool                         hit{ false };
            float                        distance;
            DirectX::SimpleMath::Vector3 position;
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
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;

        // Render the level geometry and the objects contained in this room.
        // context: The D3D context.
        // camera: The camera to use to render.
        // texture_storage: The textures for the level.
        // selected: The selection mode to use to highlight geometry and objects.
        void render(CComPtr<ID3D11DeviceContext> context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected);

        // Add the specified entity to the room.
        // Entity: The entity to add.
        void add_entity(Entity* entity);

        // Add the transparent triangles to the specified transparency buffer.
        // transparency: The buffer to add triangles to.
        void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected);
    private:
        void generate_geometry(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage);
        void generate_adjacency(const trlevel::ILevel& level, const trlevel::tr3_room& room);
        void generate_static_meshes(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage);

        struct Triangle
        {
            Triangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2)
                : v0(v0), v1(v1), v2(v2), normal((v1 - v0).Cross(v2 - v0))
            {
            }

            DirectX::SimpleMath::Vector3 v0;
            DirectX::SimpleMath::Vector3 v1;
            DirectX::SimpleMath::Vector3 v2;
            DirectX::SimpleMath::Vector3 normal;
        };

        RoomInfo                           _info;
        std::set<uint16_t>                 _neighbours;

        std::vector<std::unique_ptr<StaticMesh>> _static_meshes;

        // Rendering bits:
        CComPtr<ID3D11Device>       _device;
        std::unique_ptr<Mesh>       _mesh;
        DirectX::SimpleMath::Matrix _room_offset;

        // Triangle copy for ray intersection.
        std::vector<Triangle> _collision_triangles;
        DirectX::BoundingBox  _bounding_box;

        std::vector<Entity*> _entities;
    };
}