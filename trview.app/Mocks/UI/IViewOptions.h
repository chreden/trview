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
            MOCK_METHOD(void, set_depth, (int32_t), (override));
            MOCK_METHOD(void, set_depth_enabled, (bool), (override));
            MOCK_METHOD(void, set_flip, (bool), (override));
            MOCK_METHOD(void, set_flip_enabled, (bool), (override));
            MOCK_METHOD(void, set_highlight, (bool), (override));
            MOCK_METHOD(void, set_show_hidden_geometry, (bool), (override));
            MOCK_METHOD(void, set_show_triggers, (bool), (override));
            MOCK_METHOD(void, set_show_water, (bool), (override));
            MOCK_METHOD(void, set_show_wireframe, (bool), (override));
            MOCK_METHOD(void, set_use_alternate_groups, (bool), (override));
            MOCK_METHOD(void, set_show_bounding_boxes, (bool), (override));
            MOCK_METHOD(bool, show_hidden_geometry, (), (const, override));
            MOCK_METHOD(bool, show_triggers, (), (const, override));
            MOCK_METHOD(bool, show_water, (), (const, override));
            MOCK_METHOD(bool, show_wireframe, (), (const, override));
            MOCK_METHOD(bool, show_bounding_boxes, (), (const, override));
        };
    }
}

