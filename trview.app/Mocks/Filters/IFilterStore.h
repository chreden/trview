#pragma once

#include "../../Filters/IFilterStore.h"

namespace trview
{
    namespace mocks
    {
        struct MockFilterStore : public IFilterStore
        {
            MockFilterStore();
            virtual ~MockFilterStore();
            MOCK_METHOD(void, add, (const std::string&, const Filters::Filter&), (override));
            MOCK_METHOD(void, load, (), (override));
            MOCK_METHOD((std::map<std::string, Filters::Filter>), filters, (), (const, override));
            MOCK_METHOD((std::map<std::string, Filters::Filter>), filters_for_key, (const std::string&), (const, override));
            MOCK_METHOD(void, save, (), (override));
        };
    }
}
