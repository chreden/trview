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
            enum class InsertOrder
            {
                Column,
                Row
            };

            explicit Grid(const Size& size, const Colour& background_colour, InsertOrder insert_order, uint32_t columns, uint32_t rows);
        protected:
            virtual void inner_add_child(Control* child_element) override;
        private:
            InsertOrder _insert_order;
            Window* _grid { nullptr };
            uint32_t _rows{ 1 };
            uint32_t _columns{ 1 };
            std::vector<Window*> _cells;
        };
    }
}
