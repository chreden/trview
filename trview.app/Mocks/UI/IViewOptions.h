#pragma once

#include <trview.app/UI/IViewOptions.h>

namespace trview
{
    namespace mocks
    {
        struct MockViewOptions final : public IViewOptions
        {
            virtual ~MockViewOptions() = default;
            MOCK_METHOD(void, set_alternate_group, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_alternate_groups, (const std::set<uint32_t>&), (override));
            MOCK_METHOD(void, set_scalar, (const std::string&, int32_t), (override));
            MOCK_METHOD(void, set_flip_enabled, (bool), (override));
            MOCK_METHOD(void, set_use_alternate_groups, (bool), (override));
            MOCK_METHOD(void, set_toggle, (const std::string&, bool), (override));
            MOCK_METHOD(bool, toggle, (const std::string&), (const, override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}

