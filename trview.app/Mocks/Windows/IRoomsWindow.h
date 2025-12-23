#pragma once

#include "../../Windows/IRoomsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockRoomsWindow : public IRoomsWindow
        {
            MockRoomsWindow();
            virtual ~MockRoomsWindow();
            MOCK_METHOD(void, clear_selected_trigger, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, clear_selected_light, (), (override));
            MOCK_METHOD(void, clear_selected_camera_sink, (), (override));
            MOCK_METHOD(std::string, name, (), (const, override));
            MOCK_METHOD(void, set_filters, (std::vector<Filters<IRoom>::Filter>), (override));
        };
    }
}