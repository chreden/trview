#include "Dropdown.h"
#include "Label.h"
#include "StackPanel.h"
#include "Button.h"

namespace trview
{
    namespace ui
    {
        Dropdown::Dropdown(const Point& position, const Size& size)
            : Window(position, size, Colour(1.0f, 0.0f, 0.0f))
        {
            _label = add_child(
                std::make_unique<Label>(
                    position, 
                    size, 
                    Colour(1.0f, 0.0f, 0.0f), 
                    L"", 
                    8,
                    graphics::TextAlignment::Centre,
                    graphics::ParagraphAlignment::Centre));
            set_handles_input(true);
            set_handles_hover(true);
        }

        void Dropdown::set_dropdown_scope(ui::Control* scope)
        {
            auto dropdown = std::make_unique<StackPanel>(Point(), Size(size().width, size().height * _values.size()), Colour(0.4f, 0.0f, 0.4f), Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
            dropdown->_z = -1;
            dropdown->set_visible(false);
            _dropdown = scope->add_child(std::move(dropdown));
            update_dropdown();
        }

        void Dropdown::set_values(const std::vector<std::wstring>& values)
        {
            _values = values;
            update_dropdown();
        }

        void Dropdown::update_dropdown()
        {
            if (!_dropdown || !_dropdown->visible())
            {
                return;
            }

            // Set the position of the dropdown to be just below us.
            _dropdown->set_position(absolute_position() + Point(0, size().height));
            _dropdown->clear_child_elements();
            for (const auto& value : _values)
            {
                auto button = std::make_unique<Button>(Point(), size(), value);
                _token_store.add(button->on_click += [&]()
                {
                    _label->set_text(value);
                    _dropdown->set_visible(false);
                });
                _dropdown->add_child(std::move(button));
            }
        }

        bool Dropdown::mouse_down(const Point&)
        {
            return true;
        }

        bool Dropdown::mouse_up(const Point&)
        {
            return true;
        }

        bool Dropdown::clicked(Point)
        {
            _dropdown->set_visible(!_dropdown->visible());
            update_dropdown();
            return true;
        }
    }
}
