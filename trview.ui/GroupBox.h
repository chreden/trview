#pragma once

#include "Window.h"

namespace trview
{
    namespace ui
    {
        class GroupBox : public Window
        {
        public:
            GroupBox(Point point, Size size, Colour background_colour, Colour border_colour, const std::wstring& text);
        private:
            Colour       _border_colour;
            std::wstring _text;
        };
    }
}
