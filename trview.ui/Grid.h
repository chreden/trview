#pragma once

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

            explicit Grid(const Size& size, const Colour& background_colour, InsertOrder insert_order);
        private:
            InsertOrder _insert_order;
        };
    }
}
