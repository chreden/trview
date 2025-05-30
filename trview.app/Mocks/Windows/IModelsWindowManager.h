#pragma once

#include "../../Windows/Models/IModelsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockModelsWindowManager : public IModelsWindowManager
        {
            MockModelsWindowManager();
            virtual ~MockModelsWindowManager();
            MOCK_METHOD(std::weak_ptr<IModelsWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_level_texture_storage, (const std::weak_ptr<ILevelTextureStorage>&), (override));
            MOCK_METHOD(void, set_model_storage, (const std::weak_ptr<IModelStorage>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
