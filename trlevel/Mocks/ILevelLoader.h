#pragma once

#include "../ILevelLoader.h"

namespace trlevel
{
    namespace mocks
    {
        class MockLevelLoader final : public ILevelLoader
        {
        public:
            virtual ~MockLevelLoader() = default;
            MOCK_METHOD(std::unique_ptr<ILevel>, load_level, (const std::string&), (const));
        };
    }
}
