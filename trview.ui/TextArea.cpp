#include "TextArea.h"
#include "Label.h"
#include <sstream>

namespace trview
{
    namespace ui
    {
        TextArea::TextArea(const Point& position, const Size& size, const Colour& background_colour, const Colour& text_colour)
            : Window(position, size, background_colour), _text_colour(text_colour)
        {
            _area = add_child(std::make_unique<StackPanel>(Point(), size, background_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual));
            _area->set_margin(Size(1, 1));
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
                    // VK_BACK
                    case 0x8:
                    {
                        if (!text.empty())
                        {
                            if (_cursor_position > 0)
                            {
                                text.erase(--_cursor_position, 1);
                                notify_text_updated();
                            }
                        }
                        else
                        {
                            remove_line();
                            // Move the cursor to the end of the line that we just moved up to,
                            // as we did just do a backspace.
                            _cursor_position = current_line()->text().size();
                            return;
                        }
                        break;
                    }
                    // VK_RETURN
                    case 0xD:
                    {
                        add_line();
                        ++_cursor_line;
                        return;
                    }
                    default:
                        // Check if adding the character is going to make the text wider than the text area. If so,
                        // then create a new line and put the character on that line instead.
                        if (line->measure_text(text + static_cast<wchar_t>(character)).width > _area->size().width)
                        {
                            add_line({ character });
                            ++_cursor_line;
                            return;
                        }

                        // Add the character to the current line.
                        text.insert(text.begin() + _cursor_position, static_cast<wchar_t>(character));
                        ++_cursor_position;
                        break;
                }

                line->set_text(text);
                notify_text_updated();
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

            std::wstringstream stream(text);
            std::wstring line;
            while (std::getline(stream, line, L'\n'))
            {
                add_line(line);
            }
        }

        bool TextArea::mouse_down(const Point& position)
        {
            return true;
        }

        bool TextArea::key_down(uint16_t key)
        {
            auto line = current_line();
            auto text = line->text();

            switch (key) 
            {
                // VK_END
                case 0x23:
                {
                    _cursor_position = text.size();
                    break;
                }
                // VK_HOME
                case 0x24:
                {
                    _cursor_position = 0;
                    break;
                }
                // VK_LEFT
                case 0x25:
                {
                    if (_cursor_position > 0)
                    {
                        --_cursor_position;
                    }
                    break;
                }
                // VK_UP
                case 0x26:
                {
                    if (_cursor_line > 0)
                    {
                        --_cursor_line;
                    }
                    break;
                }
                // VK_RIGHT
                case 0x27:
                {
                    _cursor_position = std::min(static_cast<uint32_t>(text.size()), _cursor_position + 1);
                    break;
                }
                // VK_DOWN
                case 0x28:
                {
                    if (!_lines.empty())
                    {
                        _cursor_line = std::min(_cursor_line + 1, static_cast<uint32_t>(_lines.size()) - 1u);
                    }
                    break;
                }
                // VK_DELETE
                case 0x2E:
                {
                    text.erase(_cursor_position, 1);
                    line->set_text(text);
                    notify_text_updated();
                    break;
                }
            }
            update_cursor();
            return true;
        }

        Label* TextArea::current_line()
        {
            if (_lines.empty())
            {
                add_line();
            }
            return _lines[_cursor_line];
        }

        void TextArea::add_line(std::wstring text)
        {
            _lines.push_back(_area->add_child(std::make_unique<Label>(Point(), Size(size().width, 14), background_colour(), text, 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Near, SizeMode::Auto)));
            notify_text_updated();
            _cursor_position = text.size();
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
            if (_cursor_line >= _lines.size() && _cursor_line > 0)
            {
                --_cursor_line;
            }
            notify_text_updated();
        }

        void TextArea::remove_line(uint32_t line)
        {
            if (line >= _lines.size())
            {
                return;
            }

            _area->remove_child(_lines[line]);
            _lines.erase(_lines.begin() + line);
            if (_cursor_line >= _lines.size() && _cursor_line > 0)
            {
                --_cursor_line;
            }
            notify_text_updated();
        }

        void TextArea::update_cursor()
        {
            // Get the current line and the text it is rendering.
            auto line = current_line();
            auto text = line->text();

            // Place the cursor based on the current cursor position and the size of the text
            // as it would be renderered.
            auto size = line->measure_text(text.substr(0, _cursor_position));
            _cursor->set_position(Point(size.width + 2, line->position().y));
            _cursor->set_size(Size(1, line->size().height == 0 ? _cursor->size().height : line->size().height));
        }

        void TextArea::notify_text_updated()
        {
            bool first = true;
            std::wstring full_string;
            for (const auto& line : _lines)
            {
                if (!first)
                {
                    full_string += L'\n';
                }
                first = false;
                full_string += line->text();
            }
            on_text_changed(full_string);
        }
    }
}
