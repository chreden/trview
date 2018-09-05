#include "Scrollbar.h"

namespace trview
{
    namespace ui
    {
        Scrollbar::Scrollbar(const Point& position, const Size& size)
            : Window(position, size, Colour(1.0f, 0.4f, 0.4f, 0.4f))
        {
            auto blob = std::make_unique<Window>(Point(), Size(size.width, 10), Colour(1.0f, 0.2f, 0.2f, 0.2f));
            _blob = blob.get();
            add_child(std::move(blob));
        }

        void Scrollbar::set_range(float range_start, float range_end, float maximum)
        {
            float range_start_percentage = range_start / maximum;
            float range_end_percentage = range_end / maximum;
            const float range = range_end_percentage - range_start_percentage;
            const auto current_size = size();

            _blob->set_size(Size(current_size.width, range * current_size.height));

            const auto blob_height = range * current_size.height;
            _blob->set_position(Point(0, range_start_percentage * current_size.height));
        }

        bool Scrollbar::clicked(Point position)
        {
            on_scroll(position.y / size().height);
            return true;
        }
    }
}