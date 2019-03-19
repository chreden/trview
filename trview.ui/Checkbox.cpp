#include "Checkbox.h"
#include "Label.h"

namespace trview
{
    using namespace graphics;

    namespace ui
    {
        namespace
        {
            const Colour bg_colour { Colour(0.2f, 0.2f, 0.2f) };
        }

        Checkbox::Checkbox(const Point& position, const Size& size)
            : StackPanel(position, size, Colour::Transparent, Size(), Direction::Horizontal)
        {
            create_image(size, Colour(1.0f, 0.5f, 0.5f, 0.5f));
        }

        Checkbox::Checkbox(const Point& position, const Size& size, const std::wstring& label_text)
            : Checkbox(position, size, Colour(1.0f, 0.5f, 0.5f, 0.5f), label_text)
        {
        }

        Checkbox::Checkbox(const Point& position, const Size& size, const Colour& background_colour, const std::wstring& label_text)
            : StackPanel(position, size, Colour::Transparent, Size(), Direction::Horizontal)
        {
            create_image(size, background_colour);

            add_child(std::make_unique<Window>(Point(), Size(3, size.height), background_colour));

            auto label = std::make_unique<Label>(Point(), Size(1, 1), background_colour, label_text, 8, TextAlignment::Left, ParagraphAlignment::Centre, SizeMode::Auto);
            label->set_vertical_alignment(Align::Centre);
            _label = add_child(std::move(label));
        }

        void Checkbox::create_image(const Size& size, const Colour& colour)
        {
            auto outer = std::make_unique<Window>(Point(), size, Colour(0.6f, 0.6f, 0.6f));
            outer->add_child(std::make_unique<Window>(Point(1, 1), size - Size(2, 2), bg_colour));
            _check = outer->add_child(std::make_unique<Label>(Point(), size - Size(2, 2), Colour::Transparent, L"", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
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
            _check->set_text(_state ? L"X" : L"");
        }

        void Checkbox::set_enabled(bool enabled)
        {
            _enabled = enabled;
            set_handles_input(enabled);
            _label->set_text_colour(enabled ? Colour(1.0f, 1.0f, 1.0f, 1.0f) : Colour(1.0f, 0.7f, 0.7f, 0.7f));
        }
    }
}
