#include "MeshStorage.h"
#include <ranges>

namespace trview
{
    IMeshStorage::~IMeshStorage()
    {
    }

    MeshStorage::MeshStorage(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage)
    {
        const uint32_t pointers = level.num_mesh_pointers();
        for (uint32_t i = 0; i < pointers; ++i)
        {
            _meshes.insert({ i, create_mesh(level.get_mesh_by_pointer(i), mesh_source, texture_storage, level.platform_and_version()) });
        }
    }

    std::shared_ptr<IMesh> MeshStorage::mesh(uint32_t mesh_pointer) const 
    {
        auto found = _meshes.find(mesh_pointer);
        if (found != _meshes.end())
        {
            return found->second;
        }
        return nullptr;
    }

    std::map<uint32_t, std::weak_ptr<IMesh>> MeshStorage::meshes() const
    {
        std::map<uint32_t, std::weak_ptr<IMesh>> meshes;
        for (const auto& m : _meshes)
        {
            meshes[m.first] = m.second;
        }
        return meshes;
    }
}
