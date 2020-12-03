#pragma once

#include <vector>
#include "Window.h"

namespace trview
{
    namespace ui
    {
        class Grid final : public Window
        {
        public:
            explicit Grid(const Size& size, const Colour& background_colour, uint32_t columns, uint32_t rows);
        protected:
            virtual void inner_add_child(Control* child_element) override;
        private:
            Window* _grid { nullptr };
            uint32_t _rows{ 1 };
            uint32_t _columns{ 1 };
            std::vector<Window*> _cells;
        };
    }
}
