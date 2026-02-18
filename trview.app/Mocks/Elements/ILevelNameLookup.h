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
            MOCK_METHOD(std::optional<Name>, lookup, (const std::weak_ptr<ILevel>&), (const, override));
            MOCK_METHOD(std::optional<Name>, lookup, (const std::weak_ptr<trlevel::ILevel>&), (const, override));
            MOCK_METHOD(std::vector<int32_t>, bonus_items, (const std::weak_ptr<ILevel>&), (const, override));
        };
    }
}
