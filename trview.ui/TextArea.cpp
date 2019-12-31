#include "TextArea.h"
#include "Label.h"
#include <sstream>

namespace trview
{
    namespace ui
    {
        TextArea::TextArea(const Size& size, const Colour& background_colour, const Colour& text_colour, graphics::TextAlignment text_alignment)
            : TextArea(Point(), size, background_colour, text_colour, text_alignment)
        {
        }

        TextArea::TextArea(const Point& position, const Size& size, const Colour& background_colour, const Colour& text_colour, graphics::TextAlignment text_alignment)
            : Window(position, size, background_colour), _text_colour(text_colour), _alignment(text_alignment)
        {
            _area = add_child(std::make_unique<StackPanel>(size, background_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual));
            _area->set_margin(Size(1, 1));
            _cursor = add_child(std::make_unique<Window>(Size(1, 14), text_colour));
            _cursor->set_visible(focused());
            set_handles_input(true);
        }

        void TextArea::gained_focus()
        {
            Window::gained_focus();
            update_cursor();
        }

        void TextArea::lost_focus(Control* new_focus)
        {
            Window::lost_focus(new_focus);
            update_cursor();
        }

        bool TextArea::key_char(wchar_t character)
        {
            if (!focused())
            {
                return false;
            }

            auto process_char = [&]()
            {
                auto line = current_line();
                auto text = line->text();

                switch (character)
                {
                    // Reserved
                    case 0x7:
                    {
                        return;
                    }
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
                    // VK_TAB
                    case 0x9:
                    {
                        if (_mode == Mode::SingleLine)
                        {
                            on_tab(text);
                        }
                        break;
                    }
                    // VK_RETURN
                    case 0xD:
                    {
                        if (_mode == Mode::SingleLine)
                        {
                            on_enter(text);
                        }
                        else
                        {
                            add_line();
                            ++_cursor_line;
                        }
                        return;
                    }
                    // VK_ESCAPE
                    case 0x1B:
                    {
                        on_escape();
                        return;
                    }
                    default:
                    {
                        // Check if this is character we don't support.
                        if (!line->is_valid_character(character))
                        {
                            break;
                        }

                        // Check if adding the character is going to make the text wider than the text area. If so,
                        // then create a new line and put the character on that line instead.
                        if (line->measure_text(text + character).width > _area->size().width)
                        {
                            if (_mode == Mode::SingleLine)
                            {
                                return;
                            }

                            add_line({ character });
                            ++_cursor_line;
                            return;
                        }

                        // Add the character to the current line.
                        text.insert(text.begin() + _cursor_position, character);
                        ++_cursor_position;
                        break;
                    }
                }

                line->set_text(text);
                notify_text_updated();
            };

            process_char();
            update_cursor();
            return true;
        }

        void TextArea::set_text(const std::wstring& text)
        {
            while (!_lines.empty())
            {
                remove_line(false);
            }

            std::wstringstream stream(text);
            std::wstring line;
            while (std::getline(stream, line, L'\n'))
            {
                add_line(line, false);
            }

            _cursor_line = _lines.empty() ? 0 : _lines.size() - 1;
            _cursor_position = _lines.empty() ? 0 : _lines.back()->text().size();
            update_cursor(false);
        }

        void TextArea::set_mode(Mode mode)
        {
            _mode = mode;
        }

        std::wstring TextArea::text() const
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
            return full_string;
        }

        bool TextArea::mouse_down(const Point&)
        {
            return true;
        }

        bool TextArea::key_down(uint16_t key)
        {
            if (!focused())
            {
                return false;
            }

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
                    else if (_cursor_line > 0)
                    {
                        --_cursor_line;
                        _cursor_position = _lines[_cursor_line]->text().size();
                    }
                    break;
                }
                // VK_UP
                case 0x26:
                {
                    if (_cursor_line > 0)
                    {
                        --_cursor_line;
                        _cursor_position = std::min(static_cast<uint32_t>(_lines[_cursor_line]->text().size()), _cursor_position);
                    }
                    break;
                }
                // VK_RIGHT
                case 0x27:
                {
                    if (_cursor_position < text.size())
                    {
                        ++_cursor_position;
                    }
                    else if (_cursor_line + 1 < _lines.size())
                    {
                        ++_cursor_line;
                        _cursor_position = 0;
                    }
                    break;
                }
                // VK_DOWN
                case 0x28:
                {
                    if (!_lines.empty())
                    {
                        _cursor_line = std::min(_cursor_line + 1, static_cast<uint32_t>(_lines.size()) - 1u);
                        _cursor_position = std::min(static_cast<uint32_t>(_lines[_cursor_line]->text().size()), _cursor_position);
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

        Label* TextArea::current_line(bool raise_event)
        {
            if (_lines.empty())
            {
                add_line(std::wstring(), raise_event);
            }
            return _lines[_cursor_line];
        }

        void TextArea::add_line(std::wstring text, bool raise_event)
        {
            _lines.push_back(_area->add_child(std::make_unique<Label>(Size(size().width, 14), background_colour(), text, 8, _alignment, graphics::ParagraphAlignment::Near, SizeMode::Manual)));
            if (raise_event)
            {
                notify_text_updated();
            }
            _cursor_position = text.size();
            update_cursor(raise_event);
        }

        void TextArea::remove_line(bool raise_event)
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

            if (raise_event)
            {
                notify_text_updated();
            }
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

        void TextArea::update_cursor(bool raise_event)
        {
            // Get the current line and the text it is rendering.
            auto line = current_line(raise_event);
            auto text = line->text();

            // Place the cursor based on the current cursor position and the size of the text as it would be renderered.
            const auto size = line->measure_text(text.substr(0, _cursor_position));
            const auto start = _alignment == graphics::TextAlignment::Left ?
                Point(0, line->position().y) :
                Point(line->size().width * 0.5f - size.width * 0.5f - 1, line->position().y);
            _cursor->set_position(start + Point(size.width + 2, 0));
            _cursor->set_size(Size(1, line->size().height == 0 ? _cursor->size().height : line->size().height));
            _cursor->set_visible(focused());
        }

        void TextArea::notify_text_updated()
        {
            on_text_changed(text());
        }
    }
}
