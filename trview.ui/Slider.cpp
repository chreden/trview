#include "Slider.h"

namespace trview
{
    namespace ui
    {
        Slider::Slider(Point position, Size size)
            : Window(position, size, Colour(0, 0, 0, 0))
        {
            // Create the child windows (and store them for later manipulation).
            auto line = std::make_unique<Window>(
                Point(0, size.height / 2.f - 1.f), 
                Size(size.width, 2.f), 
                Colour(1, 0, 0, 0));

            auto blob = std::make_unique<Window>(
                Point(0, 0),
                Size(5, size.height),
                Colour(1, 0.2f, 0.2f, 0.2f));

            add_child(std::move(line));
            add_child(std::move(blob));
        }
    }
}