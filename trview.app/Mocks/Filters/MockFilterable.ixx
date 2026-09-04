module;

#include <gmock/gmock.h>

export module trview.app:MockFilterable;

import :IFilterable;

namespace trview
{
    namespace mocks
    {
        export struct MockFilterable : public IFilterable
        {
            MockFilterable() {}
            virtual ~MockFilterable() {}
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));
        };
    }
}
