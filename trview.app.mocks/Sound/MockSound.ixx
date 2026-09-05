module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockSound;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockSound : public ISound
        {
            MockSound(){}
            virtual ~MockSound(){}
            MOCK_METHOD(void, play, (), (override));
        };
    }
}
