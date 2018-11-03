#include "stdafx.h"
#include "MeshStorage.h"

namespace trview
{
    MeshStorage::MeshStorage(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage)
        : _device(device), _level(level), _texture_storage(texture_storage)
    {
    }

    Mesh * MeshStorage::mesh(uint32_t mesh_pointer) const 
    {
        auto found = _meshes.find(mesh_pointer);
        if (found != _meshes.end())
        {
            return found->second.get();
        }

        auto level_mesh = _level.get_mesh_by_pointer(mesh_pointer);
        auto new_mesh = create_mesh(_level.get_version(), level_mesh, _device, _texture_storage);
        Mesh* mesh = new_mesh.get();
        _meshes.insert({ mesh_pointer, std::move(new_mesh) });
        return mesh;
    }
}
