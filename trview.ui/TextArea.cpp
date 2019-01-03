#include "TextArea.h"
#include "Label.h"

namespace trview
{
    namespace ui
    {
        TextArea::TextArea(const Point& position, const Size& size, const Colour& background_colour, const Colour& text_colour)
            : StackPanel(position, size, background_colour, Size(), Direction::Vertical, SizeMode::Manual), _text_colour(text_colour)
        {
            set_handles_input(true);
        }

        void TextArea::handle_char(uint16_t character)
        {
            if (focus_control() != this)
            {
                return;
            }

            auto line = current_line();
            auto text = line->text();

            switch(character)
            {
            case 0x8:
                if (!text.empty())
                {
                    text.pop_back();
                }
                else
                {
                    remove_line();
                    return;
                }
                break;
            case 0xD:
                add_line();
                return;
            default:
                text += static_cast<wchar_t>(character);
                break;
            }

            line->set_text(text);
        }

        bool TextArea::mouse_down(const Point& position)
        {
            return true;
        }

        Label* TextArea::current_line()
        {
            if (_lines.empty())
            {
                add_line();
            }
            return _lines.back();
        }

        void TextArea::add_line()
        {
            _lines.push_back(add_child(std::make_unique<Label>(Point(), Size(size().width, 14), background_colour(), L"", 8)));
        }

        void TextArea::remove_line()
        {
            if (_lines.empty())
            {
                return;
            }

            remove_child(_lines.back());
            _lines.pop_back();
        }
    }
}
