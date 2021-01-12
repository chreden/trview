#include "Grid.h"

namespace trview
{
    namespace ui
    {
        Grid::Grid(const Size& size, const Colour& background_colour, uint32_t columns, uint32_t rows)
            : Window(size, background_colour), _columns(columns), _rows(rows)
        {
            _grid = add_child(std::make_unique<Window>(size, background_colour));
        }

        void Grid::inner_add_child(Control* child_element)
        {
            if (!_grid)
            {
                return;
            }

            auto cell = _grid->add_child(std::make_unique<Window>(
                Size(static_cast<float>(static_cast<int>(size().width / _columns)),
                     static_cast<float>(static_cast<int>(size().height / _rows))), background_colour()));

            // Calculate the coordinates based on cell index.
            float y = std::floor(static_cast<float>(_cells.size() / _columns));
            float x = _cells.size() - _columns * y;
            cell->set_position(Point((size().width / _columns) * x, (size().height / _rows) * y));

            _cells.push_back(cell);

            // Transfer the new element into the cell.
            cell->add_child(remove_child(child_element));
        }
    }
}
