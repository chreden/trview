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
            MOCK_METHOD(uint16_t, chance, (), (const, override));
            MOCK_METHOD(uint16_t, characteristics, (), (const, override));
            MOCK_METHOD(uint16_t, flags, (), (const, override));
            MOCK_METHOD(uint16_t, id, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(uint16_t, sample, (), (const, override));
            MOCK_METHOD(uint16_t, volume, (), (const, override));
        };
    }
}