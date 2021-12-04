#include "Checkbox.h"
#include "Label.h"
#include "Layouts/StackLayout.h"

namespace trview
{
    using namespace graphics;

    namespace ui
    {
        namespace
        {
            const Colour bg_colour { Colour(0.2f, 0.2f, 0.2f) };
            const Size default_size{ 16, 16 };
        }

        const std::string Checkbox::Names::text{ "text" };

        Checkbox::Checkbox()
            : Checkbox(Point())
        {
        }

        Checkbox::Checkbox(const Point& position)
            : Window(position, default_size, Colour::Transparent)
        {
            set_layout(std::make_unique<StackLayout>(0.0f, StackLayout::Direction::Horizontal));
            create_image(default_size);
        }

        Checkbox::Checkbox(const Point& position, const std::wstring& label_text)
            : Checkbox(position, Colour(1.0f, 0.5f, 0.5f, 0.5f), label_text)
        {
        }

        Checkbox::Checkbox(const Point& position, const Colour& background_colour, const std::wstring& label_text)
            : Window(position, default_size, Colour::Transparent)
        {
            set_layout(std::make_unique<StackLayout>(0.0f, StackLayout::Direction::Horizontal));
            create_image(default_size);

            add_child(std::make_unique<Window>(Size(3, default_size.height), background_colour));

            _label = add_child(std::make_unique<Label>(Size(1, 1), background_colour, label_text, 8, TextAlignment::Left, ParagraphAlignment::Centre, SizeMode::Auto));
            _label->set_vertical_alignment(Align::Centre);
            _label->set_name(Names::text);
        }

        void Checkbox::create_image(const Size& size)
        {
            auto outer = add_child(std::make_unique<Window>(size, Colour(0.6f, 0.6f, 0.6f)));
            outer->add_child(std::make_unique<Window>(Point(1, 1), size - Size(2, 2), bg_colour));
            _check = outer->add_child(std::make_unique<Label>(size - Size(2, 2), Colour::Transparent, L"", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
        }

        Checkbox::Checkbox(const Colour& background_colour, const std::wstring& label_text)
            : Checkbox(Point(), background_colour, label_text)
        {
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
            if (!_enabled)
            {
                on_focus_clear_requested();
                return true;
            }

            set_state(!_state);
            on_state_changed(_state);
            on_focus_clear_requested();
            return true;
        }

        bool Checkbox::enabled() const
        {
            return _enabled;
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
            if (_label)
            {
                _label->set_text_colour(enabled ? Colour(1.0f, 1.0f, 1.0f, 1.0f) : Colour(1.0f, 0.7f, 0.7f, 0.7f));
            }
        }

        std::wstring Checkbox::text() const
        {
            if (_label)
            {
                return _label->text();
            }
            return std::wstring();
        }
    }
}
