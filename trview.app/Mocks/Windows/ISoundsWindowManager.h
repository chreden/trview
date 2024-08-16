#pragma once

#include "../../Windows/Sounds/ISoundsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockSoundsWindowManager : public ISoundsWindowManager
        {
        public:
            MockSoundsWindowManager();
            virtual ~MockSoundsWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<ISoundsWindow>, create_window, (), (override));
            MOCK_METHOD(void, set_sound_storage, (const std::weak_ptr<ISoundStorage>&), (override));
            MOCK_METHOD(void, set_sound_sources, (const std::vector<std::weak_ptr<ISoundSource>>&), (override));
        };
    }
}
