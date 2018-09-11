#include "Checkbox.h"
#include "Label.h"

namespace trview
{
    using namespace graphics;

    namespace ui
    {
        namespace
        {
            const Colour on_colour { 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour off_colour { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        Checkbox::Checkbox(const Point& position, const Size& size)
            : StackPanel(position, size, Colour(0.0f, 0.0f, 0.0f, 0.0f), Size(), Direction::Horizontal)
        {
            create_image(size, Colour(1.0f, 0.5f, 0.5f, 0.5f));
        }

        Checkbox::Checkbox(const Point& position, const Size& size, const std::wstring& label_text)
            : Checkbox(position, size, Colour(1.0f, 0.5f, 0.5f, 0.5f), label_text)
        {
        }

        Checkbox::Checkbox(const Point& position, const Size& size, const Colour& background_colour, const std::wstring& label_text)
            : StackPanel(position, size, Colour(0.0f, 0.0f, 0.0f, 0.0f), Size(), Direction::Horizontal)
        {
            create_image(size, background_colour);

            add_child(std::make_unique<Window>(Point(), Size(3, size.height), background_colour));

            auto label = std::make_unique<Label>(Point(), Size(1, 1), background_colour, label_text, 8, TextAlignment::Left, ParagraphAlignment::Centre, SizeMode::Auto);
            label->set_vertical_alignment(Align::Centre);
            _label = label.get();
            add_child(std::move(label));
        }

        void Checkbox::create_image(const Size& size, const Colour& colour)
        {
            auto outer = std::make_unique<Window>(Point(), size, Colour(1.0f, 0.0f, 0.0f, 0.0f));
            auto inner = std::make_unique<Window>(Point(1, 1), size - Size(2, 2), colour);
            auto fill = std::make_unique<Window>(Point(1, 1), size - Size(4, 4), off_colour);

            _fill = fill.get();

            inner->add_child(std::move(fill));
            outer->add_child(std::move(inner));
            add_child(std::move(outer));
        }

        bool Checkbox::mouse_down(const Point&)
        {
            return true;
        }

        bool Checkbox::mouse_up(const Point&)
        {
            return true;
        }

        bool Checkbox::clicked(Point)
        {
            set_state(!_state);
            on_state_changed(_state);
            return true;
        }

        // Gets whether the checkbox is currently checked.
        // Returns: Whether the checkbox is currently checked.
        bool Checkbox::state() const
        {
            return _state;
        }

        // Set the state of the checkbox. This will not raise the state changed event.
        // state: The new state of the checkbox.
        void Checkbox::set_state(bool state)
        {
            _state = state;
            _fill->set_background_colour(_state ? on_colour : off_colour);
        }

        void Checkbox::set_enabled(bool enabled)
        {
            _enabled = enabled;
            set_handles_input(enabled);
            _label->set_text_colour(enabled ? Colour(1.0f, 1.0f, 1.0f, 1.0f) : Colour(1.0f, 0.7f, 0.7f, 0.7f));
        }
    }
}
