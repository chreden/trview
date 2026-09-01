export module trview.app:ModelStorage;

import std;
import std.compat;

import trlevel;

import :IModel;
import :IModelStorage;
import :IMeshStorage;

namespace trview
{
    export class ModelStorage final : public IModelStorage
    {
    public:
        explicit ModelStorage(const std::shared_ptr<IMeshStorage>& mesh_storage,
            const IModel::Source& model_source,
            const trlevel::ILevel& level);
        virtual ~ModelStorage() = default;
        std::weak_ptr<IModel> find_by_type_id(uint16_t type_id) const override;
    private:
        void load_models(const std::shared_ptr<IMeshStorage>& mesh_storage, 
            const IModel::Source& model_source,
            const trlevel::ILevel& level);

        std::vector<std::shared_ptr<IModel>> _models;
        trlevel::PlatformAndVersion _platform_and_version;
    };
}
