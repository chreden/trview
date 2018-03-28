#pragma once

#include <cstdint>
#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>

namespace trlevel
{
    struct ILevel;
    struct tr2_entity;
}

namespace trview
{
    struct IMeshStorage;
    struct ILevelTextureStorage;
    class Mesh;

    class Entity
    {
    public:
        explicit Entity(CComPtr<ID3D11Device> device, const trlevel::ILevel& level, const trlevel::tr2_entity& room, const ILevelTextureStorage& texture_storage, const IMeshStorage& mesh_storage);
        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, const ILevelTextureStorage& texture_storage, const DirectX::XMFLOAT4& colour);
        uint16_t room() const;
    private:
        CComPtr<ID3D11Device> _device;

        DirectX::XMMATRIX               _world;
        std::vector<Mesh*>              _meshes;
        std::vector<DirectX::XMMATRIX>  _world_transforms;
        uint16_t                        _room;
    };
}
