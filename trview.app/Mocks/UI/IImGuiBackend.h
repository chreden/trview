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
            MOCK_METHOD(void, new_frame, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, shutdown, (), (override));
            MOCK_METHOD(bool, window_procedure, (HWND, UINT, WPARAM, LPARAM), (override));
        };
    }
}
