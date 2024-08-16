#pragma once

#include <trview.app/Elements/SoundSource/ISoundSource.h>

namespace trview
{
    namespace mocks
    {
        struct MockSoundSource : public ISoundSource, public std::enable_shared_from_this<MockSoundSource>
        {
            MockSoundSource();
            ~MockSoundSource();
            MOCK_METHOD(uint32_t, number, (), (const, override));
        };
    }
}