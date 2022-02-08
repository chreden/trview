#pragma once

#include "../../UI/IMapRenderer.h"

namespace trview
{
    namespace ui
    {
        namespace render
        {
            namespace mocks
            {
                struct MockMapRenderer : public IMapRenderer
                {
                    virtual ~MockMapRenderer() = default;
                    MOCK_METHOD(void, render, ());
                    MOCK_METHOD(void, load, (const std::shared_ptr<trview::IRoom>&));
                    MOCK_METHOD(std::uint16_t, area, (), (const));
                    MOCK_METHOD(std::shared_ptr<ISector>, sector_at, (const Point&), (const));
                    MOCK_METHOD(std::shared_ptr<ISector>, sector_at_cursor, (), (const));
                    MOCK_METHOD(bool, cursor_is_over_control, (), (const));
                    MOCK_METHOD(void, set_cursor_position, (const Point&));
                    MOCK_METHOD(bool, loaded, (), (const));
                    MOCK_METHOD(void, set_window_size, (const Size&));
                    MOCK_METHOD(void, set_visible, (bool));
                    MOCK_METHOD(void, clear_highlight, ());
                    MOCK_METHOD(void, set_highlight, (uint16_t, uint16_t));
                    MOCK_METHOD(graphics::Texture, texture, (), (const));
                    MOCK_METHOD(Point, first, (), (const));
                    MOCK_METHOD(void, set_render_mode, (RenderMode), (override));
                };
            }
        }
    }
}

