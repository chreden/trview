#pragma once

#include <cstdint>
#include <wrl/client.h>
#include <d3d11.h>
#include <SimpleMath.h>

#include <memory>
#include <vector>


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

    class Entity
    {
    public:
        explicit Entity(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const trlevel::ILevel& level, const trlevel::tr2_entity& room, const ILevelTextureStorage& texture_storage, const IMeshStorage& mesh_storage);
        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour, float scale = 1.0f);
        uint16_t room() const;

        void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour);
        void set_selected(bool value);
    private:
        void load_meshes(const trlevel::ILevel& level, int16_t type_id, const IMeshStorage& mesh_storage);
        void load_model(const trlevel::tr_model& model, const trlevel::ILevel& level);
        void load_sprite(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const trlevel::tr_sprite_sequence& sprite_sequence, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage);

        DirectX::SimpleMath::Matrix               _world;
        std::vector<Mesh*>                        _meshes;
        std::unique_ptr<Mesh>                     _sprite_mesh;
        std::vector<DirectX::SimpleMath::Matrix>  _world_transforms;
        uint16_t                                  _room;
        bool                                      _selected{ false };

        // Bits for sprites.
        DirectX::SimpleMath::Matrix               _scale;
        DirectX::SimpleMath::Matrix               _offset;
        DirectX::SimpleMath::Vector3              _position;
    };
}
