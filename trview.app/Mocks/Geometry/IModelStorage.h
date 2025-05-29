#pragma once

#include "../../Geometry/Model/IModelStorage.h"

namespace trview
{
    namespace mocks
    {
        struct MockModelStorage : public IModelStorage
        {
            MockModelStorage();
            virtual ~MockModelStorage();
            MOCK_METHOD(std::weak_ptr<IModel>, find_by_type_id, (uint16_t), (const, override));
        };
    }
}