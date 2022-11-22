#include "MeshStorage.h"

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
            _meshes.insert({ i, create_mesh(level.get_mesh_by_pointer(i), mesh_source, texture_storage) });
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
}
