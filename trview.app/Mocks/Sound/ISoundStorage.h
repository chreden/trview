#pragma once

#include "../../Sound/ISoundStorage.h"

namespace trview
{
    namespace mocks
    {
        struct MockSoundStorage : public ISoundStorage
        {
            MockSoundStorage();
            virtual ~MockSoundStorage();
            MOCK_METHOD(void, add, (ISoundStorage::Index, const std::vector<uint8_t>&), (override));
            MOCK_METHOD(std::weak_ptr<ISound>, get, (uint16_t), (const, override));
            MOCK_METHOD(std::vector<Entry>, sounds, (), (const, override));
        };
    }
}
