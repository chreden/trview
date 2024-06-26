#pragma once

#include "../../UI/IImGuiBackend.h"

namespace trview
{
    namespace mocks
    {
        struct MockImGuiBackend : public IImGuiBackend
        {
            MockImGuiBackend();
            virtual ~MockImGuiBackend();
            MOCK_METHOD(void, initialise, (), (override));
            MOCK_METHOD(bool, is_setup, (), (const, override));
            MOCK_METHOD(void, new_frame, (), (override));
            MOCK_METHOD(void, rebuild_fonts, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, reset_layout, (), (override));
            MOCK_METHOD(void, shutdown, (), (override));
            MOCK_METHOD(bool, window_procedure, (HWND, UINT, WPARAM, LPARAM), (override));
        };
    }
}
