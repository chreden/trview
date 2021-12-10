#include "Scrollbar.h"

namespace trview
{
    namespace ui
    {
        const std::string Scrollbar::Names::blob{ "blob" };

        Scrollbar::Scrollbar(const Size& size, const Colour& background_colour)
            : Scrollbar(Point(), size, background_colour)
        {
        }

        Scrollbar::Scrollbar(const Point& position, const Size& size, const Colour& background_colour)
            : Window(position, size, background_colour)
        {
            _blob = add_child(std::make_unique<Window>(Point(1, 1), Size(size.width - 2, 10), Colour(0.2f, 0.2f, 0.2f)));
            _blob->set_name(Names::blob);
        }

        void Scrollbar::set_range(float range_start, float range_end, float maximum)
        {
            if (!maximum)
            {
                _blob->set_visible(false);
                return;
            }

            float range_start_percentage = range_start / maximum;
            float range_end_percentage = range_end / maximum;
            const float range = range_end_percentage - range_start_percentage;
            const auto current_size = size();

            const auto blob_height = range * current_size.height;
            _blob->set_size(Size(current_size.width - 2, std::max(1.0f, blob_height - 2)));
            _blob->set_position(Point(1, range_start_percentage * current_size.height + 1));
            _blob->set_visible(blob_height != current_size.height);
        }

        bool Scrollbar::mouse_down(const Point&)
        {
            return true;
        }

        bool Scrollbar::mouse_up(const Point&)
        {
            on_focus_clear_requested();
            return true;
        }

        bool Scrollbar::move(Point position)
        {
            Control::move(position);
            if (_input && _input->focus_control() == this)
            {
                return clicked(position);
            }
            return false;
        }

        bool Scrollbar::clicked(Point position)
        {
            on_scroll(std::clamp(position.y / size().height, 0.0f, 1.0f));
            return true;
        }
    }
}