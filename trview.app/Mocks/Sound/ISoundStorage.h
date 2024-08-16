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
            MOCK_METHOD(void, add, (uint16_t, const std::vector<uint8_t>&), (override));
            MOCK_METHOD(std::weak_ptr<ISound>, get, (uint16_t), (const, override));
            MOCK_METHOD((std::unordered_map<uint16_t, std::weak_ptr<ISound>>), sounds, (), (const, override));
        };
    }
}
