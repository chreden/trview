#include "Slider.h"

namespace trview
{
    namespace ui
    {
        namespace
        {
            const float BlobWidth = 10.f;
        }

        Slider::Slider(Point position, Size size)
            : Window(position, size, Colour(0, 0, 0, 0))
        {
            // Create the child windows (and store them for later manipulation).
            auto line = std::make_unique<Window>(
                Point(BlobWidth / 2.f, size.height / 2.f - 1.f),
                Size(size.width - BlobWidth, 2.f),
                Colour(1, 0, 0, 0));

            auto blob = std::make_unique<Window>(
                Point(0, 0),
                Size(BlobWidth, size.height),
                Colour(1, 0.2f, 0.2f, 0.2f));

            _blob = blob.get();

            add_child(std::move(line));
            add_child(std::move(blob));

            set_blob_position(Point(0, 0));
        }

        bool Slider::clicked(Point position)
        {
            set_blob_position(position);
            return true;
        }

        void Slider::set_blob_position(Point position)
        {
            const auto pos = _blob->position();
            _blob->set_position(Point(position.x - BlobWidth / 2.0f, pos.y));
        }
    }
}