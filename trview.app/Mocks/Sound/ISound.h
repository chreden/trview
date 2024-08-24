#pragma once

#include "../../Sound/ISound.h"

namespace trview
{
    namespace mocks
    {
        struct MockSound : public ISound
        {
            MockSound();
            virtual ~MockSound();
            MOCK_METHOD(void, play, (), (override));
        };
    }
}
