module;

#include <gmock/gmock.h>
#include <SimpleMath.h>

export module trview.app:MockLevelNameLookup;

import :ILevelNameLookup;

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
