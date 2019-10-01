#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <unordered_map>
#include <cstdint>
#include <memory>

#include <trlevel/ILevel.h>

#include "IMeshStorage.h"
#include <trview.app/Geometry/Mesh.h>
#include <trview.graphics/Device.h>

namespace trview
{
    struct ILevelTextureStorage;

    class MeshStorage final : public IMeshStorage
    {
    public:
        explicit MeshStorage(const graphics::Device& device, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage);

        virtual ~MeshStorage() = default;

        virtual Mesh* mesh(uint32_t mesh_pointer) const override;
    private:
        const graphics::Device& _device;
        const ILevelTextureStorage& _texture_storage;
        mutable std::unordered_map<uint32_t, std::unique_ptr<Mesh>> _meshes;
    };
}
