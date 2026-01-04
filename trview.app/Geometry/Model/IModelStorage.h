#pragma once

namespace trview
{
    struct IModel;
    struct IModelStorage
    {
        virtual ~IModelStorage() = 0;
        virtual std::weak_ptr<IModel> find_by_type_id(uint16_t type_id) const = 0;
        virtual std::vector<std::weak_ptr<IModel>> models() const = 0;
    };
}
