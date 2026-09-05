module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockModelStorage;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockModelStorage : public IModelStorage
        {
            MockModelStorage(){}
            virtual ~MockModelStorage(){}
            MOCK_METHOD(std::weak_ptr<IModel>, find_by_type_id, (uint16_t), (const, override));
        };
    }
}