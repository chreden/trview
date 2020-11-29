#include "Grid.h"

namespace trview
{
    namespace ui
    {
        Grid::Grid(const Size& size, const Colour& background_colour, InsertOrder insert_order)
            : Window(size, background_colour), _insert_order(insert_order)
        {
        }
    }
}
