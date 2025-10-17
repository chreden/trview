#pragma once

#include "../../UI/IMapRenderer.h"

namespace trview
{
    namespace mocks
    {
        struct MockMapRenderer : public IMapRenderer
        {
            MockMapRenderer();
            virtual ~MockMapRenderer();
            MOCK_METHOD(void, render, (bool), (override));
            MOCK_METHOD(void, load, (const std::shared_ptr<trview::IRoom>&), (override));
            MOCK_METHOD(bool, cursor_is_over_control, (), (const, override));
            MOCK_METHOD(bool, loaded, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(void, clear_highlight, (), (override));
            MOCK_METHOD(void, set_highlight, (uint16_t, uint16_t), (override));
            MOCK_METHOD(void, set_mode, (Mode), (override));
            MOCK_METHOD(void, set_selection, (const std::shared_ptr<ISector>&), (override));
            MOCK_METHOD(void, set_show_tooltip, (bool), (override));
            MOCK_METHOD(Size, size, (), (const, override));
            MOCK_METHOD(void, reposition, (), (override));
            MOCK_METHOD(void, reset, (), (override));
        };
    }
}

