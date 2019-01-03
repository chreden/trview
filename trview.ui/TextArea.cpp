#include "TextArea.h"
#include "Label.h"

namespace trview
{
    namespace ui
    {
        TextArea::TextArea(const Point& position, const Size& size, const Colour& background_colour, const Colour& text_colour)
            : Window(position, size, background_colour), _text_colour(text_colour)
        {
            _area = add_child(std::make_unique<StackPanel>(Point(), size, background_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual));
            _cursor = add_child(std::make_unique<Window>(Point(), Size(1, 14), text_colour));
            set_handles_input(true);
        }

        void TextArea::handle_char(uint16_t character)
        {
            if (focus_control() != this)
            {
                return;
            }

            auto process_char = [&]()
            {
                auto line = current_line();
                auto text = line->text();

                switch (character)
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
            };

            process_char();
            update_cursor();
        }

        void TextArea::set_text(const std::wstring& text)
        {
            while (!_lines.empty())
            {
                remove_line();
            }
            add_line(text);
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

        void TextArea::add_line(std::wstring text)
        {
            _lines.push_back(_area->add_child(std::make_unique<Label>(Point(), Size(size().width, 14), background_colour(), text, 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Near, SizeMode::Auto)));
            update_cursor();
        }

        void TextArea::remove_line()
        {
            if (_lines.empty())
            {
                return;
            }

            _area->remove_child(_lines.back());
            _lines.pop_back();
        }

        void TextArea::update_cursor()
        {
            auto line = current_line();
            _cursor->set_position(Point(line->size().width + 2, line->position().y));
            _cursor->set_size(Size(1, line->size().height == 0 ? _cursor->size().height : line->size().height));
        }
    }
}
