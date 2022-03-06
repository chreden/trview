#pragma once

#include "../../UI/IMapRenderer.h"

namespace trview
{
    namespace mocks
    {
        struct MockMapRenderer : public IMapRenderer
        {
            virtual ~MockMapRenderer() = default;
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, load, (const std::shared_ptr<trview::IRoom>&), (override));
            MOCK_METHOD(std::uint16_t, area, (), (const, override));
            MOCK_METHOD(std::shared_ptr<ISector>, sector_at, (const Point&), (const, override));
            MOCK_METHOD(std::shared_ptr<ISector>, sector_at_cursor, (), (const, override));
            MOCK_METHOD(bool, cursor_is_over_control, (), (const, override));
            MOCK_METHOD(void, set_cursor_position, (const Point&), (override));
            MOCK_METHOD(bool, loaded, (), (const, override));
            MOCK_METHOD(void, set_window_size, (const Size&), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(void, clear_highlight, (), (override));
            MOCK_METHOD(void, set_highlight, (uint16_t, uint16_t), (override));
            MOCK_METHOD(graphics::Texture, texture, (), (const, override));
            MOCK_METHOD(Point, first, (), (const, override));
            MOCK_METHOD(void, set_render_mode, (RenderMode), (override));
            MOCK_METHOD(void, set_colours, (const MapColours&), (override));
        };
    }
}

