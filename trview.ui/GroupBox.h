#pragma once

#include "Window.h"

namespace trview
{
    namespace ui
    {
        class Label;

        class GroupBox : public Window
        {
        public:
            GroupBox(const Point& point, const Size& size, const Colour& background_colour, const Colour& border_colour, const std::wstring& text);
            std::wstring title() const;
            void set_title(const std::wstring& title);
        private:
            Colour _border_colour;
            Label* _label;
        };
    }
}
