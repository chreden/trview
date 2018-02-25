#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <unordered_map>
#include <cstdint>
#include <memory>

#include <trlevel/ILevel.h>

#include "IMeshStorage.h"
#include "Mesh.h"

namespace trview
{
    struct ITextureStorage;

    class MeshStorage final : public IMeshStorage
    {
    public:
        explicit MeshStorage(CComPtr<ID3D11Device> device, const trlevel::ILevel& level, const ITextureStorage& texture_storage);

        virtual ~MeshStorage() = default;

        virtual Mesh* mesh(uint32_t mesh_pointer) const override;
    private:
        CComPtr<ID3D11Device> _device;
        const trlevel::ILevel& _level;
        const ITextureStorage& _texture_storage;
        mutable std::unordered_map<uint16_t, std::unique_ptr<Mesh>> _meshes;
    };
}
