#pragma once

#include "../../Windows/Sounds/ISoundsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockSoundsWindow : public ISoundsWindow
        {
            MockSoundsWindow();
            virtual ~MockSoundsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_level_platform, (trlevel::Platform), (override));
            MOCK_METHOD(void, set_level_version, (trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_sound_storage, (const std::weak_ptr<ISoundStorage>&), (override));
            MOCK_METHOD(void, set_sound_sources, (const std::vector<std::weak_ptr<ISoundSource>>&), (override));
        };
    }
}