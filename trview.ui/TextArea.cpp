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
            if (_text.empty())
            {
                _text.push_back({});
            }

            if (any_text_selected())
            {
                delete_selection();
            }

            auto filtered_text = text;
            // Remove \r of \r\n
            filtered_text.erase(std::remove(filtered_text.begin(), filtered_text.end(), L'\r'), filtered_text.end());

            if (_mode == Mode::SingleLine)
            {
                filtered_text.erase(std::remove(filtered_text.begin(), filtered_text.end(), L'\n'), filtered_text.end());

                auto& current_text = _text[_logical_cursor.line];
                current_text.insert(_logical_cursor.position, text);
                _logical_cursor.position += text.size();
                notify_text_updated();
                update_structure();
                return true;
            }

            std::wstringstream stream(filtered_text);
            std::vector<std::wstring> lines;
            std::wstring line;
            while (std::getline(stream, line, L'\n'))
            {
                lines.push_back(line);
            }

            for (auto i = 0u; i < lines.size(); ++i)
            {
                auto& current_text = _text[_logical_cursor.line];
                current_text.insert(_logical_cursor.position, lines[i]);
                _logical_cursor.position += lines[i].size();
                if (i < lines.size() - 1)
                {
                    new_line();
                }
            }

            notify_text_updated();
            update_structure();
            return true;
        }

        void TextArea::update_structure()
        {
            uint32_t count = 0;

            // Gets the next line or add a new one if required.
            auto get_line = [&]()
            {
                if (_lines.size() <= count)
                {
                    _lines.push_back(_area->add_child(std::make_unique<Label>(Size(size().width, 14), background_colour(), L"", 8, _alignment, graphics::ParagraphAlignment::Near, SizeMode::Manual)));
                    on_hierarchy_changed();
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
                    if (_mode == Mode::MultiLine && line_label->measure_text(line).width > line_label->size().width)
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

            // Remove any extra lines
            if (_lines.size() > _line_structure.size())
            {
                for (auto i = _line_structure.size(); i < _lines.size(); ++i)
                {
                    _area->remove_child(_lines[i]);
                }
                _lines.erase(_lines.begin() + _line_structure.size(), _lines.end());
                on_hierarchy_changed();
            }

            // Render the cursor at the correct position?
            // Translate logical cursor to visual cursor:
            auto iter = std::find_if(_line_structure.begin(), _line_structure.end(),
                [&](const auto& ls) { return ls.line == _logical_cursor.line; });

            if (iter != _line_structure.end())
            {
                auto remaining = _logical_cursor.position;
                while (remaining > iter->length)
                {
                    remaining -= iter->length;
                    ++iter;
                }
                _visual_cursor.line = iter - _line_structure.begin();
                _visual_cursor.position = remaining;
            }
            else
            {
                _visual_cursor.line = 0u;
                _visual_cursor.position = 0;
            }

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

            auto& current = _text[_logical_cursor.line];

            switch (character)
            {
                // VK_BACK
                case 0x8:
                {
                    if (any_text_selected())
                    {
                        delete_selection();
                    }
                    else
                    {
                        if (_logical_cursor.position > 0)
                        {
                            // Just remove a character.
                            current.erase(_logical_cursor.position - 1, 1);
                            --_logical_cursor.position;
                        }
                        else if (_logical_cursor.line > 0)
                        {
                            // Remove a line.
                            _logical_cursor.position = _text[_logical_cursor.line - 1].size();
                            _text[_logical_cursor.line - 1] += _text[_logical_cursor.line];
                            _text.erase(_text.begin() + _logical_cursor.line);
                            --_logical_cursor.line;
                        }
                    }
                    break;
                }
                // VK_TAB
                case 0x9:
                {
                    if (_mode == Mode::SingleLine)
                    {
                        on_tab(text());
                    }
                    return true;
                }
                // VK_RETURN
                case 0xD:
                {
                    if (any_text_selected())
                    {
                        delete_selection();
                    }

                    if (_mode == Mode::SingleLine)
                    {
                        on_enter(text());
                        return true;
                    }
                    else
                    {
                        new_line();
                    }
                    break;
                }
                // Select All,
                case 0x1:
                {
                    if (_text.empty())
                    {
                        highlight({ 0u, 0u }, { 0u, 0u });
                    }
                    else
                    {
                        const auto end = CursorPoint{ static_cast<uint32_t>(_text.size()) - 1, static_cast<uint32_t>(_text.back().size()) };
                        highlight({ 0u, 0u }, end);
                        move_visual_cursor_position(end.line, end.position);
                    }
                    break;
                }
                // Copy, Undo, Redo, Cut, Paste
                case 0x3:
                case 0x7:
                case 0x1a:
                case 0x16:
                case 0x18:
                case 0x19:
                {
                    return true;
                }
                // VK_ESCAPE
                case 0x1B:
                {
                    on_escape();
                    return true;
                }
                default:
                {
                    if (any_text_selected())
                    {
                        delete_selection();
                    }

                    auto& text = _text[_logical_cursor.line];
                    text.insert(text.begin() + _logical_cursor.position, character);
                    ++_logical_cursor.position;
                    break;
                }
            }

            notify_text_updated();
            update_structure();
            return true;
        }

        void TextArea::set_text(const std::wstring& text)
        {
            std::wstringstream stream(text);
            std::wstring line;

            _text.clear();
            while (std::getline(stream, line, L'\n'))
            {
                _text.push_back(line);
            }

            _logical_cursor.line = _text.empty() ? 0 : _text.size() - 1;
            _logical_cursor.position = _text.empty() ? 0 : _text.back().size();
            notify_text_updated();
            update_structure();
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
            for (const auto& text : _text)
            {
                if (!first)
                {
                    full_string += L'\n';
                }
                first = false;
                full_string += text;
            }
            return full_string;
        }

        bool TextArea::mouse_down(const Point&)
        {
            return true;
        }

        void TextArea::move_visual_cursor_position(uint32_t line, uint32_t position)
        {
            _visual_cursor.line = std::clamp<uint32_t>(line, 0u, static_cast<int32_t>(_line_structure.size()) - 1);
            _visual_cursor.position = std::clamp<uint32_t>(position, 0u, static_cast<int32_t>(_line_structure[_visual_cursor.line].length));
            _logical_cursor.line = _line_structure[_visual_cursor.line].line;
            _logical_cursor.position = _line_structure[_visual_cursor.line].start + _visual_cursor.position;
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

        bool TextArea::key_down(uint16_t key, bool control_pressed, bool shift_pressed)
        {
            if (!focused())
            {
                return false;
            }

            auto line = current_line();

            switch (key) 
            {
                // VK_END
                case 0x23:
                {
                    if (shift_pressed)
                    {
                        highlight(_visual_cursor, { _visual_cursor.line, _line_structure[_visual_cursor.line].length });
                    }
                    move_visual_cursor_position(_visual_cursor.line, _line_structure[_visual_cursor.line].length);
                    break;
                }
                // VK_HOME
                case 0x24:
                { 
                    if (shift_pressed)
                    {
                        highlight(_visual_cursor, { _visual_cursor.line, 0u });
                    }
                    move_visual_cursor_position(_visual_cursor.line, 0u);
                    break;
                }
                // VK_LEFT
                case 0x25:
                {
                    if (_visual_cursor.position > 0)
                    {
                        if (shift_pressed)
                        {
                            if (_selection_start == _selection_end)
                            {
                                _selection_start = _visual_cursor;
                            }
                            highlight(_selection_start, { _visual_cursor.line, _visual_cursor.position - 1 });
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_earliest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position(_visual_cursor.line, _visual_cursor.position - 1);
                        }
                    }
                    else if (_visual_cursor.line > 0)
                    {
                        if (shift_pressed)
                        {
                            if (_selection_start == _selection_end)
                            {
                                _selection_start = _visual_cursor;
                            }
                            highlight(_selection_start, { _visual_cursor.line - 1, _line_structure[_visual_cursor.line - 1].length });
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_earliest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position(_visual_cursor.line - 1, _line_structure[_visual_cursor.line - 1].length);
                        }
                    }
                    break;
                }
                // VK_UP
                case 0x26:
                {
                    if (_visual_cursor.line > 0)
                    {
                        if (shift_pressed)
                        {
                            if (_selection_start == _selection_end)
                            {
                                _selection_start = _visual_cursor;
                            }
                            highlight(_selection_start,
                                { _visual_cursor.line - 1, find_nearest_index(_visual_cursor.line - 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width) });
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            highlight(_selection_end, _selection_end);
                        }

                        move_visual_cursor_position(_visual_cursor.line - 1, 
                            find_nearest_index(_visual_cursor.line - 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width));
                    }
                    break;
                }
                // VK_RIGHT
                case 0x27:
                {
                    if (_visual_cursor.position < _line_structure[_visual_cursor.line].length)
                    {
                        if (shift_pressed)
                        {
                            if (_selection_start == _selection_end)
                            {
                                _selection_start = _visual_cursor;
                            }
                            highlight(_selection_start, { _visual_cursor.line, _visual_cursor.position + 1 });
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_latest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position(_visual_cursor.line, _visual_cursor.position + 1);
                        }
                    }
                    else if ((_visual_cursor.line + 1) < _line_structure.size())
                    {
                        if (shift_pressed)
                        {
                            if (_selection_start == _selection_end)
                            {
                                _selection_start = _visual_cursor;
                            }
                            highlight(_selection_start, { _visual_cursor.line + 1, 0u });
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_latest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position(_visual_cursor.line + 1, 0u);
                        }
                    }
                    break;
                }
                // VK_DOWN
                case 0x28:
                {
                    if (_visual_cursor.line < _line_structure.size() - 1)
                    {
                        if (shift_pressed)
                        {
                            if (_selection_start == _selection_end)
                            {
                                _selection_start = _visual_cursor;
                            }
                            highlight(_selection_start, 
                                {_visual_cursor.line + 1, find_nearest_index(_visual_cursor.line + 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width)});
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            highlight(_selection_end, _selection_end);
                        }

                        move_visual_cursor_position(_visual_cursor.line + 1,
                            find_nearest_index(_visual_cursor.line + 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width));
                    }
                    break;
                }
                // VK_DELETE
                case 0x2E:
                {
                    if (any_text_selected())
                    {
                        delete_selection();
                    }
                    else
                    {
                        if (_logical_cursor.position == _text[_logical_cursor.line].size() &&
                            _text.size() > _logical_cursor.line + 1u)
                        {
                            _text[_logical_cursor.line] += _text[_logical_cursor.line + 1];
                            _text.erase(_text.begin() + _logical_cursor.line + 1);
                        }
                        else
                        {
                            _text[_logical_cursor.line].erase(_logical_cursor.position, 1);
                        }

                        notify_text_updated();
                        update_structure();
                    }
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
                _lines.push_back(_area->add_child(std::make_unique<Label>(Size(size().width, 14), background_colour(), L"", 8, _alignment, graphics::ParagraphAlignment::Near, SizeMode::Manual)));
            }
            return _lines[_visual_cursor.line];
        }

        void TextArea::new_line()
        {
            auto& current = _text[_logical_cursor.line];
            auto remainder = current.substr(_logical_cursor.position);
            current = current.substr(0, _logical_cursor.position);
            _text.insert(_text.begin() + _logical_cursor.line + 1, remainder);
            ++_logical_cursor.line;
            _logical_cursor.position = 0u;
        }

        void TextArea::highlight(CursorPoint start, CursorPoint end)
        {
            _selection_start = start;
            _selection_end = end;

            for (auto& line : _lines)
            {
                if (!line->child_elements().empty())
                {
                    line->child_elements().back()->set_visible(false);
                }
                else
                {
                    line->add_child(std::make_unique<Window>(line->size(), Colour(0.75f, 0.0f, 1.0f, 0.0f)))->set_visible(false);
                }
            }

            if (_line_structure.empty())
            {
                return;
            }

            const auto earliest = start < end ? start : end;
            const auto latest = start < end ? end : start;
            const CursorPoint visual_start = logical_to_visual(earliest);
            const CursorPoint visual_end = logical_to_visual(latest);
            for (uint32_t i = visual_start.line; i <= visual_end.line; ++i)
            {
                auto line = _lines[i];
                auto highlight = line->child_elements().back();

                Point highlight_pos;
                Size highlight_size;
                auto text = line->text();

                if (i == visual_start.line)
                {
                    if (i == visual_end.line)
                    {
                        highlight_size = Size(line->measure_text(text.substr(visual_start.position, visual_end.position - visual_start.position)).width, line->size().height);
                    }
                    else
                    {
                        highlight_size = Size(line->measure_text(text.substr(visual_start.position)).width, line->size().height);
                    }
                    highlight_pos = Point(line->measure_text(text.substr(0, visual_start.position)).width, 0);
                }
                else if (i == visual_end.line)
                {
                    highlight_size = Size(line->measure_text(text.substr(0, visual_end.position)).width, line->size().height);
                }
                else
                {
                    highlight_size = Size(line->measure_text(text).width, line->size().height);
                }

                highlight->set_position(highlight_pos);
                highlight->set_size(highlight_size);
                highlight->set_visible(true);
            }

            on_hierarchy_changed();
        }

        void TextArea::move_to_earliest_highlight()
        {
            const auto lowest = _selection_start < _selection_end ? _selection_start : _selection_end;
            highlight(lowest, lowest);
            move_visual_cursor_position(lowest.line, lowest.position);
        }

        void TextArea::move_to_latest_highlight()
        {
            const auto highest = _selection_start < _selection_end ? _selection_end : _selection_start;
            highlight(highest, highest);
            move_visual_cursor_position(highest.line, highest.position);
        }

        void TextArea::update_cursor()
        {
            // Get the current line and the text it is rendering.
            auto line = current_line();
            auto text = line->text();

            // Place the cursor based on the current cursor position and the size of the text as it would be renderered.
            const auto size = line->measure_text(text.substr(0, _visual_cursor.position));
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

        TextArea::CursorPoint TextArea::logical_to_visual(CursorPoint point) const
        {
            const auto iter = std::find_if(_line_structure.begin(), _line_structure.end(), [&](const auto& entry)
                {
                    return point.line == entry.line && point.position >= entry.start && point.position <= entry.start + entry.length;
                });
            return { static_cast<uint32_t>(iter - _line_structure.begin()), point.position - iter->start };
        }

        TextArea::CursorPoint TextArea::visual_to_logical(CursorPoint point) const
        {
            return { _line_structure[point.line].line, _line_structure[point.line].start + point.position };
        }

        void TextArea::delete_selection()
        {
            const auto start = visual_to_logical(_selection_start);
            const auto end = visual_to_logical(_selection_end);
            const auto earlier = start < end ? start : end;
            const auto later = start < end ? end : start;

            for (int i = later.line; i >= static_cast<int>(earlier.line); --i)
            {
                if (i == later.line)
                {
                    if (i == earlier.line)
                    {
                        _text[i].erase(_text[i].begin() + earlier.position, _text[i].begin() + later.position);
                    }
                    else
                    {
                        _text[i].erase(_text[i].begin(), _text[i].begin() + later.position);
                    }
                }
                else if (i == earlier.line)
                {
                    _text[i].erase(_text[i].begin() + earlier.position, _text[i].end());
                    _text[i] += _text[i + 1];
                    _text.erase(_text.begin() + i + 1);
                }
                else
                {
                    _text.erase(_text.begin() + i);
                }
            }

            highlight(earlier, earlier);
            move_visual_cursor_position(earlier.line, earlier.position);
            notify_text_updated();
            update_structure();
        }

        bool TextArea::any_text_selected() const
        {
            return _selection_start != _selection_end;
        }
    }
}
