#pragma once

#include "Window.h"

namespace trview
{
    namespace ui
    {
        class GroupBox : public Window
        {
        public:
            GroupBox(const Point& point, const Size& size, const Colour& background_colour, const Colour& border_colour, const std::wstring& text);
        private:
            Colour       _border_colour;
        };
    }
}
