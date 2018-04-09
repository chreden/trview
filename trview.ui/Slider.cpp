#include "Slider.h"
#include <algorithm>

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
                Point(BlobWidth, size.height / 2.f - 1.f),
                Size(size.width - BlobWidth * 2.f, 2.f),
                Colour(1, 0, 0, 0));

            auto blob = std::make_unique<Window>(
                Point(0, 0),
                Size(BlobWidth, size.height),
                Colour(1.0f, 0.2f, 0.2f, 0.2f));

            _blob = blob.get();

            add_child(std::move(line));
            add_child(std::move(blob));

            set_blob_position(Point(0, 0));
        }

        void Slider::set_value(float value)
        {
            _value = value;
            set_blob_position(value);
        }

        bool Slider::clicked(Point position)
        {
            set_blob_position(position);
            return true;
        }

        bool Slider::move(Point position)
        {
            if (focus_control() == this)
            {
                set_blob_position(position);
                return true;
            }
            return false;
        }

        void Slider::set_blob_position(float percentage)
        {
            const float SliderSize = size().width - BlobWidth * 2;
            const float x = BlobWidth + percentage * SliderSize - BlobWidth * 0.5f;

            const auto pos = _blob->position();
            _blob->set_position(Point(x, pos.y));
            on_value_changed(percentage);
        }

        void Slider::set_blob_position(Point position)
        {
            const float SliderSize = size().width - BlobWidth * 2;
            const float percentage = std::min(1.0f, std::max(0.0f, (position.x - BlobWidth) / SliderSize));
            set_blob_position(percentage);
        }
    }
}