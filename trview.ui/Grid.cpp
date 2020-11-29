#include "Grid.h"

namespace trview
{
    namespace ui
    {
        Grid::Grid(const Size& size, const Colour& background_colour, InsertOrder insert_order)
            : Window(size, background_colour), _insert_order(insert_order)
        {
            _grid = add_child(std::make_unique<Window>(size, background_colour));
        }

        void Grid::inner_add_child(Control* child_element)
        {
            // Take the newly added child element, add it to the 'grid'
            if (!_grid)
            {
                return;
            }

            const int Columns = 2;
            const int Rows = 3;

            // Add the cell to the grid
            auto cell = _grid->add_child(std::make_unique<Window>(Size(static_cast<int>(size().width / Columns), static_cast<int>(size().height / Rows)), background_colour()));

            // Calculate the coordinates based on cell index.
            auto y = std::floor(_cells.size() / Columns);
            auto x = _cells.size() - Columns * y;
            cell->set_position(Point((size().width / Columns) * x, (size().height / Rows) * y));

            _cells.push_back(cell);

            // Transfer the new element into the cell.
            cell->add_child(remove_child(child_element));
        }
    }
}
