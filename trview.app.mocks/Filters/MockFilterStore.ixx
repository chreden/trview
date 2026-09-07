module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockFilterStore;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockFilterStore : public IFilterStore
        {
            MOCK_METHOD(void, add, (const std::string&, const Filters::Filter&), (override));
            MOCK_METHOD(void, load, (), (override));
            MOCK_METHOD((std::map<std::string, Filters::Filter>), filters_for_key, (const std::string&), (const, override));
            MOCK_METHOD(void, remove, (const std::string&, const std::string&), (override));
            MOCK_METHOD(void, save, (), (override));
        };
    }
}
