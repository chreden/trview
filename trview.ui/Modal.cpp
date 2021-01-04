#include "Modal.h"

namespace trview
{
    namespace ui
    {
        Modal::Modal(const Size& size, const Colour& background_colour)
            : Window(size, background_colour)
        {
            set_handles_input(true);
            set_handles_hover(true);
            set_z(-2);
            set_visible(false);
        }

        bool Modal::mouse_down(const Point& position)
        {
            return true;
        }

        bool Modal::mouse_up(const Point& position)
        {
            return true;
        }

        bool Modal::clicked(Point position)
        {
            return true;
        }
    }
}
