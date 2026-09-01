export module trview.app:MeshStorage;

import std;

import trlevel;

import :IMeshStorage;
import :IMesh;

namespace trview
{
    struct ILevelTextureStorage;

    export class MeshStorage final : public IMeshStorage
    {
    public:
        explicit MeshStorage(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const ILevelTextureStorage& texture_storage);
        virtual ~MeshStorage() = default;
        virtual std::shared_ptr<IMesh> mesh(uint32_t mesh_pointer) const override;
    private:
        mutable std::unordered_map<uint32_t, std::shared_ptr<IMesh>> _meshes;
    };
}
