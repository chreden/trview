#pragma once

#include <unordered_map>
#include <cstdint>
#include <memory>
#include <trlevel/ILevel.h>
#include "IMeshStorage.h"
#include <trview.app/Geometry/IMesh.h>

namespace trview
{
    struct ILevelTextureStorage;

    class MeshStorage final : public IMeshStorage
    {
    public:
        explicit MeshStorage(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage);
        virtual ~MeshStorage() = default;
        virtual std::shared_ptr<IMesh> mesh(uint32_t mesh_pointer) const override;
        virtual std::map<uint32_t, std::weak_ptr<IMesh>> meshes() const override;
    private:
        mutable std::unordered_map<uint32_t, std::shared_ptr<IMesh>> _meshes;
    };
}
