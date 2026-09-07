export module trview.app:IModelStorage;

import std;
import std.compat;

import :IModel;

namespace trview
{
    export struct IModelStorage
    {
        virtual ~IModelStorage() = 0;
        virtual std::weak_ptr<IModel> find_by_type_id(uint16_t type_id) const = 0;
    };
}
