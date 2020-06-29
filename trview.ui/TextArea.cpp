#include "TextArea.h"
#include "Label.h"
#include <sstream>
#include <queue>

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

        bool TextArea::paste(const std::wstring& text)
        {
            auto line = current_line();
            auto line_text = line->text();
            // line_text.insert(line_text.begin() + _cursor_position, text.begin(), text.end());
            // _cursor_position += text.size();
            line->set_text(line_text);
            update_cursor();
            return true;
        }

        void TextArea::update_structure()
        {
            // The number of lines that exist.
            uint32_t count = 0;

            // Get the next line or add a new one if required.
            auto get_line = [&]()
            {
                if (_lines.size() <= count)
                {
                    _lines.push_back(_area->add_child(std::make_unique<Label>(Size(size().width, 14), background_colour(), L"", 8, _alignment, graphics::ParagraphAlignment::Near, SizeMode::Manual)));
                }
                return _lines[count++];
            };

            auto process_line = [&](uint32_t index, const std::wstring& text_line)
            {
                std::queue<std::wstring> lines_to_process;
                lines_to_process.push(text_line);
                uint32_t character_count = 0;

                while (!lines_to_process.empty())
                {
                    auto line = lines_to_process.front();
                    lines_to_process.pop();

                    auto line_label = get_line();
                    if (line_label->measure_text(line).width > line_label->size().width)
                    {
                        auto split_length = 0u;
                        while (line_label->measure_text(line.substr(0, split_length + 1)).width < _area->size().width)
                        {
                            ++split_length;
                        }

                        // Hackery with the index - might need to be fixed.
                        line_label->set_text(line.substr(0, split_length));
                        _line_structure.push_back({ index, character_count, split_length });
                        lines_to_process.push(line.substr(split_length));
                        character_count += split_length;
                    }
                    else
                    {
                        // No splitting required.
                        line_label->set_text(line);
                        _line_structure.push_back({ index, character_count, static_cast<uint32_t>(line.size()) });
                    }
                }
            };

            _line_structure.clear();
            for (auto i = 0u; i < _text.size(); ++i)
            {
                process_line(i, _text[i]);
            }

            // Remove any extra lines.
            if (_lines.size() > _line_structure.size())
            {
                for (auto i = _line_structure.size(); i < _lines.size(); ++i)
                {
                    _area->remove_child(_lines[i]);
                }
                _lines.erase(_lines.begin() + _line_structure.size(), _lines.end());
            }

            // Render the cursor at the correct position?
            // Translate logical cursor to visual cursor:
            auto iter = std::find_if(_line_structure.begin(), _line_structure.end(),
                [&](const auto& ls) { return ls.line == _logical_cursor_line; });

            auto remaining = _logical_cursor_position;
            while (remaining > iter->length)
            {
                remaining -= iter->length;
                ++iter;
            }
            _visual_cursor_line = iter - _line_structure.begin();
            _visual_cursor_position = remaining;
            update_cursor();
        }

        bool TextArea::key_char(wchar_t character)
        {
            if (!focused())
            {
                return false;
            }

            if (_text.empty())
            {
                _text.push_back({});
            }

            auto& current = _text[_logical_cursor_line];

            switch (character)
            {
                // VK_RETURN
                case 0xD:
                {
                    auto remainder = current.substr(_logical_cursor_position);
                    current = current.substr(0, _logical_cursor_position);
                    _text.insert(_text.begin() + _logical_cursor_line + 1, remainder);
                    ++_logical_cursor_line;
                    _logical_cursor_position = 0u;
                    break;
                }
                default:
                {
                    auto& text = _text[_logical_cursor_line];
                    text.insert(text.begin() + _logical_cursor_position, character);
                    ++_logical_cursor_position;
                    break;
                }
            }

            update_structure();
            return true;
            /*

            auto line = current_line();
            auto text = line->text();

            text.insert(text.begin() + _cursor_position, character);
            ++_cursor_position;
            line->set_text(text);

            notify_text_updated();
            update_cursor();
            return true;*/

            /*
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
            */
        }

        void TextArea::set_text(const std::wstring& text)
        {
            std::wstringstream stream(text);
            std::vector<std::wstring> newlines;
            std::wstring line;
            while (std::getline(stream, line, L'\n'))
            {
                newlines.push_back(line);
            }

            if (newlines.size() == _lines.size())
            {
                for (std::size_t i = 0; i < newlines.size(); ++i)
                {
                    _lines[i]->set_text(newlines[i]);
                }
            }
            else
            {
                while (!_lines.empty())
                {
                    remove_line(false);
                }

                for (const auto& line : newlines)
                {
                    add_line(line, false);
                }
            }

            // _cursor_line = _lines.empty() ? 0 : _lines.size() - 1;
            // _cursor_position = _lines.empty() ? 0 : _lines.back()->text().size();
            update_cursor(false);
        }

        void TextArea::set_mode(Mode mode)
        {
            if (_mode == mode)
            {
                return;
            }

            _mode = mode;
            if (_mode == TextArea::Mode::SingleLine)
            {
                set_text(text());
            }
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

        void TextArea::move_visual_cursor_position(uint32_t line, uint32_t position)
        {
            _visual_cursor_line = std::clamp<uint32_t>(line, 0u, static_cast<int32_t>(_line_structure.size()) - 1);
            _visual_cursor_position = std::clamp<uint32_t>(position, 0u, static_cast<int32_t>(_line_structure[_visual_cursor_line].length));
            _logical_cursor_line = _line_structure[_visual_cursor_line].line;
            _logical_cursor_position = _line_structure[_visual_cursor_line].start + _visual_cursor_position;
        }

        uint32_t TextArea::find_nearest_index(uint32_t index, float x) const
        {
            auto line = _lines[index];
            auto text = line->text();

            float previous = 0.0f;
            for (auto i = 0u; i < text.size(); ++i)
            {
                float current = line->measure_text(text.substr(0, i + 1)).width;
                if (x < current)
                {
                    if (((x - previous) / (current - previous)) >= 0.5f)
                    {
                        return i + 1;
                    }
                    else
                    {
                        return i;
                    }
                }
                previous = current;
            }
            return text.size();
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
                    move_visual_cursor_position(_visual_cursor_line, _line_structure[_visual_cursor_line].length);
                    break;
                }
                // VK_HOME
                case 0x24:
                { 
                    move_visual_cursor_position(_visual_cursor_line, 0u);
                    break;
                }
                // VK_LEFT
                case 0x25:
                {
                    if (_visual_cursor_position > 0)
                    {
                        move_visual_cursor_position(_visual_cursor_line, _visual_cursor_position - 1);
                    }
                    else if (_visual_cursor_line > 0)
                    {
                        move_visual_cursor_position(_visual_cursor_line - 1, _line_structure[_visual_cursor_line - 1].length);
                    }
                    break;
                }
                // VK_UP
                case 0x26:
                {
                    if (_visual_cursor_line > 0)
                    {
                        move_visual_cursor_position(_visual_cursor_line - 1, 
                            find_nearest_index(_visual_cursor_line - 1, line->measure_text(line->text().substr(0, _visual_cursor_position)).width));
                    }
                    break;
                }
                // VK_RIGHT
                case 0x27:
                {
                    if (_visual_cursor_position < _line_structure[_visual_cursor_line].length)
                    {
                        move_visual_cursor_position(_visual_cursor_line, _visual_cursor_position + 1);
                    }
                    else if ((_visual_cursor_line + 1) < _line_structure.size())
                    {
                        move_visual_cursor_position(_visual_cursor_line + 1, 0u);
                    }
                    break;
                }
                // VK_DOWN
                case 0x28:
                {
                    if (_visual_cursor_line < _line_structure.size() - 1)
                    {
                        move_visual_cursor_position(_visual_cursor_line + 1, 
                            find_nearest_index(_visual_cursor_line + 1, line->measure_text(line->text().substr(0, _visual_cursor_position)).width));                        
                    }
                    break;
                }
                // VK_DELETE
                case 0x2E:
                {
                    if (_logical_cursor_position == _text[_logical_cursor_line].size() &&
                        _text.size() > _logical_cursor_line + 1u)
                    {
                        _text[_logical_cursor_line] += _text[_logical_cursor_line + 1];
                        _text.erase(_text.begin() + _logical_cursor_line + 1);
                    }
                    else
                    {
                        _text[_logical_cursor_line].erase(_logical_cursor_position, 1);
                    }
                    update_structure();
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
            return _lines[_visual_cursor_line];
        }

        void TextArea::add_line(std::wstring text, bool raise_event)
        {
            _lines.push_back(_area->add_child(std::make_unique<Label>(Size(size().width, 14), background_colour(), text, 8, _alignment, graphics::ParagraphAlignment::Near, SizeMode::Manual)));
            if (raise_event)
            {
                notify_text_updated();
            }
            // _cursor_position = text.size();
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
            /*
            if (_cursor_line >= _lines.size() && _cursor_line > 0)
            {
                --_cursor_line;
            }
            */

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
            /*
            if (_cursor_line >= _lines.size() && _cursor_line > 0)
            {
                --_cursor_line;
            }
            */
            notify_text_updated();
        }

        void TextArea::update_cursor(bool raise_event)
        {
            // Get the current line and the text it is rendering.
            auto line = current_line(raise_event);
            auto text = line->text();

            // Place the cursor based on the current cursor position and the size of the text as it would be renderered.
            const auto t = text.substr(0, _visual_cursor_position);
            const auto size = line->measure_text(t);
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
