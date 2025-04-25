#pragma once

#include "../../Elements/Level/ILevelNameLookup.h"

namespace trview
{
    namespace mocks
    {
        struct MockLevelNameLookup : public ILevelNameLookup
        {
            MockLevelNameLookup();
            virtual ~MockLevelNameLookup();
            MOCK_METHOD(std::optional<std::string>, lookup, (const std::weak_ptr<ILevel>&), (const, override));
        };
    }
}
