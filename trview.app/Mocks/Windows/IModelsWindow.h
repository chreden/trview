#pragma once

#include "../../Windows/Models/IModelsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockModelsWindow : public IModelsWindow
        {
            MockModelsWindow();
            virtual ~MockModelsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_model_storage, (const std::weak_ptr<IModelStorage>&), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}
