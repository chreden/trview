#pragma once

#include <trlevel/ILevel.h>

#include "IModelStorage.h"
#include "IModel.h"

namespace trview
{
    struct IMeshStorage;
    class ModelStorage final : public IModelStorage
    {
    public:
        explicit ModelStorage(const std::shared_ptr<IMeshStorage>& mesh_storage,
            const IModel::Source& model_source,
            const trlevel::ILevel& level);
        virtual ~ModelStorage() = default;
        std::weak_ptr<IModel> find_by_type_id(uint16_t type_id) const override;
        std::vector<std::weak_ptr<IModel>> models() const override;
    private:
        void load_models(const std::shared_ptr<IMeshStorage>& mesh_storage, 
            const IModel::Source& model_source,
            const trlevel::ILevel& level);

        std::vector<std::shared_ptr<IModel>> _models;
        trlevel::PlatformAndVersion _platform_and_version;
    };
}
