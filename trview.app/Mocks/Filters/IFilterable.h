#pragma once

#include "../../Filters/IFilterable.h"

namespace trview
{
    namespace mocks
    {
        struct MockFilterable : public IFilterable
        {
            MockFilterable();
            virtual ~MockFilterable();
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));
        };
    }
}
