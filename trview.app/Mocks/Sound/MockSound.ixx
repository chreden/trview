module;

#include <gmock/gmock.h>

export module trview.app:MockSound;

import :ISound;

namespace trview
{
    namespace mocks
    {
        export struct MockSound : public ISound
        {
            MockSound();
            virtual ~MockSound();
            MOCK_METHOD(void, play, (), (override));
        };
    }
}
