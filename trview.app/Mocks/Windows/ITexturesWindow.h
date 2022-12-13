#pragma once

#include "../../Windows/Textures/ITexturesWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockTexturesWindow : public ITexturesWindow
        {
            MockTexturesWindow();
            virtual ~MockTexturesWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_texture_storage, (const std::shared_ptr<ILevelTextureStorage>&), (override));
        };
    }
}
