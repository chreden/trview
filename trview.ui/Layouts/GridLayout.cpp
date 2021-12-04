#include "GridLayout.h"
#include "../Control.h"

namespace trview
{
    namespace ui
    {
        GridLayout::GridLayout(uint32_t columns, uint32_t rows)
            : _columns(columns), _rows(rows)
        {
        }

        void GridLayout::bind(Control& control)
        {
            _token_store += control.on_add_child += [&](auto child)
            {
                float y = std::floor(static_cast<float>(_cells / _columns));
                float x = _cells - _columns * y;
                child->set_position(Point((control.size().width / _columns) * x, (control.size().height / _rows) * y));
                ++_cells;
            };
        }

        uint32_t GridLayout::columns() const
        {
            return _columns;
        }

        uint32_t GridLayout::rows() const
        {
            return _rows;
        }
    }
}
