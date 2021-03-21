#include "MeshStorage.h"

namespace trview
{
    MeshStorage::MeshStorage(const std::shared_ptr<graphics::IDevice>& device, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage)
        : _device(device), _texture_storage(texture_storage)
    {
        const uint32_t pointers = level.num_mesh_pointers();
        for (uint32_t i = 0; i < pointers; ++i)
        {
            auto level_mesh = level.get_mesh_by_pointer(i);
            auto new_mesh = create_mesh(level.get_version(), level_mesh, *_device, _texture_storage);
            _meshes.insert({ i, std::move(new_mesh) });
        }
    }

    Mesh * MeshStorage::mesh(uint32_t mesh_pointer) const 
    {
        auto found = _meshes.find(mesh_pointer);
        if (found != _meshes.end())
        {
            return found->second.get();
        }
        return nullptr;
    }
}
