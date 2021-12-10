#include "Slider.h"

namespace trview
{
    namespace ui
    {
        namespace
        {
            const float BlobWidth = 10.f;
        }

        const std::string Slider::Names::blob{ "blob" };

        Slider::Slider(Point position, Size size)
            : Window(position, size, Colour::Transparent)
        {
            // Create the child windows (and store them for later manipulation).
            auto line = std::make_unique<Window>(
                Point(BlobWidth, size.height / 2.f - 1.f),
                Size(size.width - BlobWidth * 2.f, 2.f),
                Colour::LightGrey);

            add_child(std::move(line));

            _blob = add_child(std::make_unique<Window>(
                Point(0, 0),
                Size(BlobWidth, size.height),
                Colour::LightGrey));
            _blob->set_name(Names::blob);

            set_blob_position(Point(0, 0));
        }

        Slider::Slider(const Size& size)
            : Slider(Point(), size)
        {
        }

        float Slider::value() const
        {
            return _value;
        }

        void Slider::set_value(float value)
        {
            set_blob_position(value);
        }

        bool Slider::mouse_down(const Point&)
        {
            return true;
        }

        bool Slider::mouse_up(const Point&)
        {
            on_focus_clear_requested();
            return true;
        }

        bool Slider::clicked(Point position)
        {
            set_blob_position(position, true);
            return true;
        }

        bool Slider::move(Point position)
        {
            Control::move(position);
            if (_input && _input->focus_control() == this)
            {
                set_blob_position(position, true);
                return true;
            }
            return false;
        }

        void Slider::set_blob_position(float percentage, bool raise_event)
        {
            _value = percentage;

            const float SliderSize = size().width - BlobWidth * 2;
            const float x = BlobWidth + percentage * SliderSize - BlobWidth * 0.5f;

            const auto pos = _blob->position();
            _blob->set_position(Point(x, pos.y));
            if (raise_event)
            {
                on_value_changed(percentage);
            }
            on_invalidate();
        }

        void Slider::set_blob_position(Point position, bool raise_event)
        {
            const float SliderSize = size().width - BlobWidth * 2;
            const float percentage = std::min(1.0f, std::max(0.0f, (position.x - BlobWidth) / SliderSize));
            set_blob_position(percentage, raise_event);
        }
    }
}