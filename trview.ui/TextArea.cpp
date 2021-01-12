#include "TextArea.h"
#include "Label.h"

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
            _area = add_child(std::make_unique<StackPanel>(Size(size.width - 10, size.height), background_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual));
            _area->set_margin(Size(1, 1));
            _cursor = add_child(std::make_unique<Window>(Size(1, 14), text_colour));
            _cursor->set_visible(focused());
            _scrollbar = add_child(std::make_unique<Scrollbar>(Point(size.width - 10, 0), Size(10, _area->size().height), Colour::Grey));
            _scrollbar->set_visible(false);
            set_handles_input(true);

            // Create all line labels to fill the area.
            const auto line_height = 14;
            const auto line_count = static_cast<uint32_t>(_area->size().height / line_height);
            for (auto i = 0u; i < line_count; ++i)
            {
                _lines.push_back(_area->add_child(std::make_unique<Label>(Size(_area->size().width, line_height), this->background_colour(), L"", 8, _alignment, graphics::ParagraphAlignment::Near, SizeMode::Manual)));
            }
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
            if (read_only())
            {
                return false;
            }

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
                _logical_cursor.position += static_cast<int32_t>(text.size());
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
                _logical_cursor.position += static_cast<int32_t>(lines[i].size());
                if (i < lines.size() - 1)
                {
                    new_line();
                }
            }

            notify_text_updated();
            update_structure();
            scroll_cursor_into_view(true);
            return true;
        }

        bool TextArea::copy(std::wstring& output)
        {
            if (!any_text_selected())
            {
                return false;
            }

            output = selected_text();
            return true;
        }

        bool TextArea::cut(std::wstring& output)
        {
            if (read_only() || !any_text_selected())
            {
                return false;
            }

            output = selected_text();
            delete_selection();
            return true;
        }

        bool TextArea::clicked(Point position)
        {
            if (_lines.empty() || _line_structure.empty())
            {
                return true;
            }

            // In case the dragging had started already, clear it.
            _dragging = false;
            _selection_end = _selection_start;

            auto link = word_at_cursor(visual_to_logical(position_to_visual(position)));
            if (is_link(link))
            {
                ShellExecute(0, 0, link.c_str(), 0, 0, SW_SHOW);
            }
            return true;
        }

        bool TextArea::scroll(int delta)
        {
            _scroll_offset = std::max(0, _scroll_offset + (delta > 0 ? -1 : 1));
            update_structure();
            highlight(_selection_start, _selection_end);
            return true;
        }

        std::wstring TextArea::word_at_cursor(LogicalPosition point) const
        {
            auto& line = _text[point.line];
            if (!std::isspace(line[point.position]))
            {
                auto rstart = std::find_if(std::make_reverse_iterator(line.begin() + point.position), line.rend(), std::isspace);
                auto diff = -(rstart - line.rend());
                auto start = line.begin() + diff;
                auto end = std::find_if(line.begin() + point.position, line.end(), std::isspace);
                return std::wstring(start, end);
            }
            return std::wstring();
        }

        void TextArea::scroll_cursor_into_view(bool down)
        {
            // If already in view, don't do anything.
            if (_visual_cursor.line >= _scroll_offset && _visual_cursor.line < _scroll_offset + _lines.size())
            {
                return;
            }

            if (down)
            {
                int32_t top = std::max(0, (_visual_cursor.line + 1) - static_cast<int32_t>(_lines.size()));
                _scroll_offset = top;
            }
            else
            {
                _scroll_offset = _visual_cursor.line;
            }

            update_structure();
            highlight(_selection_start, _selection_end);
        }

        void TextArea::set_scrollbar_visible(bool value)
        {
            _scrollbar->set_visible(value);
        }

        void TextArea::update_structure()
        {
            // Use the first line label to measure text.
            const auto line_label = _lines[0];

            auto process_line = [&](int32_t index, const std::wstring& text_line)
            {
                std::queue<std::wstring> lines_to_process;
                lines_to_process.push(text_line);
                int32_t character_count = 0;

                while (!lines_to_process.empty())
                {
                    auto line = lines_to_process.front();
                    lines_to_process.pop();

                    if (_mode == Mode::MultiLine && line_label->measure_text(line).width > line_label->size().width)
                    {
                        int32_t split_length = 0;
                        while (line_label->measure_text(line.substr(0, split_length + 1)).width < _area->size().width)
                        {
                            ++split_length;
                        }

                        auto text = line.substr(0, split_length);
                        _line_structure.push_back({ index, character_count, split_length, text });
                        lines_to_process.push(line.substr(split_length));
                        character_count += split_length;
                    }
                    else
                    {
                        // No splitting required.
                        _line_structure.push_back({ index, character_count, static_cast<int32_t>(line.size()), line });
                    }
                }
            };

            _line_structure.clear();
            for (auto i = 0u; i < _text.size(); ++i)
            {
                process_line(i, _text[i]);
            }

            // Clear everything first.
            for (auto line : _lines)
            {
                line->set_text(L"");
            }

            _scroll_offset = std::max(0, std::min(_scroll_offset, static_cast<int32_t>(_line_structure.size()) - 1));
            for (auto i = _scroll_offset, j = 0; i  < _line_structure.size() && j < _lines.size(); ++i, ++j)
            {
                if (i < _line_structure.size())
                {
                    _lines[j]->set_text(_line_structure[i].text);
                }
                else
                {
                    _lines[j]->set_text(L"");
                }
            }

            _visual_cursor = logical_to_visual(_logical_cursor);
            update_cursor();
            _scrollbar->set_range(
                static_cast<float>(_scroll_offset), 
                static_cast<float>(std::min<int32_t>(_scroll_offset + static_cast<int32_t>(_lines.size()), static_cast<int32_t>(_line_structure.size()))), 
                static_cast<float>(_line_structure.size()));
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
                    if (read_only())
                    {
                        return false;
                    }

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
                            _logical_cursor.position = static_cast<int32_t>(_text[_logical_cursor.line - 1].size());
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
                    if (_mode == Mode::SingleLine || read_only())
                    {
                        on_tab(text());
                    }
                    return true;
                }
                // \n (ctrl + enter)
                case 0xA:
                {
                    auto word = word_at_cursor(_logical_cursor);
                    if (is_link(word))
                    {
                        ShellExecute(0, 0, word.c_str(), 0, 0, SW_SHOW);
                    }
                    return true;
                }
                // VK_RETURN
                case 0xD:
                {
                    if (read_only())
                    {
                        return false;
                    }

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
                    if (_text.empty() || _line_structure.empty())
                    {
                        highlight({ 0, 0 }, { 0, 0 });
                    }
                    else
                    {
                        const auto end = LogicalPosition{ static_cast<int32_t>(_text.size()) - 1, static_cast<int32_t>(_text.back().size()) };
                        highlight(end, { 0, 0 });
                        move_visual_cursor_position({ 0, 0 });
                    }
                    break;
                }
                // Copy, Ctrl + E, Undo, Redo, Cut, Paste
                case 0x3:
                case 0x5:
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
                    if (read_only())
                    {
                        return false;
                    }

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
            scroll_cursor_into_view(true);
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

            _logical_cursor.line = _text.empty() ? 0 : static_cast<int32_t>(_text.size() - 1);
            _logical_cursor.position = _text.empty() ? 0 : static_cast<int32_t>(_text.back().size());
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

        bool TextArea::mouse_down(const Point& position)
        {
            if (_lines.empty() || _line_structure.empty())
            {
                return true;
            }

            _dragging = true;
            _selection_start = visual_to_logical(position_to_visual(position));
            _selection_end = _selection_start;
            return true;
        }

        bool TextArea::mouse_up(const Point&)
        {
            _dragging = false;
            return true;
        }

        bool TextArea::move(Point position)
        {
            if (_lines.empty() || _line_structure.empty())
            {
                return true;
            }

            if (_dragging)
            {
                const auto visual = position_to_visual(position);
                highlight(_selection_start, visual_to_logical(visual));
                move_visual_cursor_position(visual);
                update_cursor();
            }
            return true;
        }

        void TextArea::move_visual_cursor_position(TextArea::VisualPosition position)
        {
            const auto visual_line = std::min<int32_t>(position.line, static_cast<int32_t>(_line_structure.size()) - 1);
            if (visual_line >= _line_structure.size())
            {
                return;
            }

            _visual_cursor.line = visual_line;
            _visual_cursor.position = std::min<int32_t>(position.position, _line_structure[visual_line].length);
            _logical_cursor = visual_to_logical(position);
        }

        int32_t TextArea::find_nearest_index(int32_t index, float x) const
        {
            if (index < 0 || index >= _line_structure.size())
            {
                return _line_structure.back().length;
            }

            auto line = _lines[0];
            auto text = _line_structure[index].text;

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
            return static_cast<int32_t>(text.size());
        }

        bool TextArea::key_down(uint16_t key, bool control_pressed, bool shift_pressed)
        {
            if (!focused())
            {
                return false;
            }

            if (_line_structure.empty())
            {
                return true;
            }

            auto line = _lines[0];

            switch (key) 
            {
                // VK_END
                case 0x23:
                {
                    const auto new_end = control_pressed ?
                        VisualPosition { static_cast<int32_t>(_line_structure.size()) - 1, _line_structure.back().length } :
                        VisualPosition { _visual_cursor.line, _line_structure[_visual_cursor.line].length };
                    if (shift_pressed)
                    {
                        highlight(any_text_selected() ? _selection_start : _logical_cursor, visual_to_logical(new_end));
                    }
                    else
                    {
                        clear_highlight();
                    }
                    move_visual_cursor_position(new_end);
                    scroll_cursor_into_view(true);
                    break;
                }
                // VK_HOME
                case 0x24:
                { 
                    const auto end = VisualPosition{ control_pressed ? 0 : _visual_cursor.line, 0 };
                    if (shift_pressed)
                    {
                        highlight(any_text_selected() ? _selection_start : _logical_cursor, visual_to_logical(end));
                    }
                    else
                    {
                        clear_highlight();
                    }
                    move_visual_cursor_position(end);
                    scroll_cursor_into_view(false);
                    break;
                }
                // VK_LEFT
                case 0x25:
                {
                    if (_visual_cursor.position > 0)
                    {
                        auto end = VisualPosition { _visual_cursor.line, _visual_cursor.position - 1 };
                        if (shift_pressed)
                        {
                            if (!any_text_selected())
                            {
                                _selection_start = _logical_cursor;
                            }

                            if (control_pressed)
                            {
                                bool found_non_whitespace = false;
                                const auto& text = _text[_logical_cursor.line];
                                for (int32_t i = _logical_cursor.position - 1; i >= 0; --i)
                                {
                                    if (i == 0)
                                    {
                                        end = logical_to_visual({ _logical_cursor.line, 0u });
                                        break;
                                    }

                                    if (text[i] == L' ')
                                    {
                                        if (found_non_whitespace)
                                        {
                                            end = logical_to_visual({ _logical_cursor.line, i + 1});
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        found_non_whitespace = true;
                                    }
                                }
                            }

                            highlight(_selection_start, visual_to_logical(end));
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_earliest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position(end);
                        }
                    }
                    else if (_visual_cursor.line > 0)
                    {
                        if (shift_pressed)
                        {
                            if (!any_text_selected())
                            {
                                _selection_start = _logical_cursor;
                            }
                            highlight(_selection_start, visual_to_logical({ _visual_cursor.line - 1, _line_structure[_visual_cursor.line - 1].length }));
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_earliest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position({ _visual_cursor.line - 1, _line_structure[_visual_cursor.line - 1].length });
                            scroll_cursor_into_view(false);
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
                            if (!any_text_selected())
                            {
                                _selection_start = _logical_cursor;
                            }
                            highlight(_selection_start,
                                visual_to_logical({ _visual_cursor.line - 1, find_nearest_index(_visual_cursor.line - 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width) }));
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            clear_highlight();
                        }

                        move_visual_cursor_position({ _visual_cursor.line - 1,
                            find_nearest_index(_visual_cursor.line - 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width)});
                        scroll_cursor_into_view(false);
                    }
                    break;
                }
                // VK_RIGHT
                case 0x27:
                {
                    if (_visual_cursor.position < _line_structure[_visual_cursor.line].length)
                    {
                        auto end = VisualPosition{ _visual_cursor.line, _visual_cursor.position + 1 };
                        if (shift_pressed)
                        {
                            if (!any_text_selected())
                            {
                                _selection_start = _logical_cursor;
                            }

                            if (control_pressed)
                            {
                                bool found_non_whitespace = false;
                                const auto& text = _text[_logical_cursor.line];
                                for (int32_t i = _logical_cursor.position; i < text.size(); ++i)
                                {
                                    if (i == text.size() - 1)
                                    {
                                        end = logical_to_visual({ _logical_cursor.line, static_cast<int32_t>(text.size()) });
                                        break;
                                    }

                                    if (text[i] == L' ')
                                    {
                                        if (found_non_whitespace)
                                        {
                                            end = logical_to_visual({ _logical_cursor.line, i });
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        found_non_whitespace = true;
                                    }
                                }
                            }

                            highlight(_selection_start, visual_to_logical(end));
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_latest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position(end);
                        }
                    }
                    else if ((_visual_cursor.line + 1) < _line_structure.size())
                    {
                        if (shift_pressed)
                        {
                            if (!any_text_selected())
                            {
                                _selection_start = _logical_cursor;
                            }
                            highlight(_selection_start, visual_to_logical({ _visual_cursor.line + 1, 0u }));
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            move_to_latest_highlight();
                        }
                        else
                        {
                            move_visual_cursor_position({ _visual_cursor.line + 1, 0u });
                            scroll_cursor_into_view(true);
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
                            if (!any_text_selected())
                            {
                                _selection_start = _logical_cursor;
                            }
                            highlight(_selection_start, 
                                visual_to_logical({_visual_cursor.line + 1, find_nearest_index(_visual_cursor.line + 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width)}));
                        }

                        if (!shift_pressed && any_text_selected())
                        {
                            highlight(_selection_end, _selection_end);
                        }

                        move_visual_cursor_position({ _visual_cursor.line + 1,
                            find_nearest_index(_visual_cursor.line + 1, line->measure_text(line->text().substr(0, _visual_cursor.position)).width)});
                        scroll_cursor_into_view(true);
                    }
                    break;
                }
                // VK_DELETE
                case 0x2E:
                {
                    if (read_only())
                    {
                        return false;
                    }

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
            auto index = _visual_cursor.line - _scroll_offset;
            if (index < _lines.size())
            {
                return _lines[index];
            }
            return nullptr;
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

        void TextArea::clear_highlight()
        {
            highlight(_selection_start, _selection_start);
        }

        void TextArea::highlight(LogicalPosition start, LogicalPosition end)
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

            if (_line_structure.empty() || !any_text_selected())
            {
                return;
            }

            const auto earliest = start < end ? start : end;
            const auto latest = start < end ? end : start;
            const VisualPosition visual_start = logical_to_visual(earliest);
            const VisualPosition visual_end = logical_to_visual(latest);
            for (int32_t i = visual_start.line; i <= visual_end.line; ++i)
            {
                int32_t element_index = i - _scroll_offset;
                if (element_index < 0 || element_index >= _lines.size())
                {
                    continue;
                }

                auto line = _lines[element_index];
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
            move_visual_cursor_position(logical_to_visual(lowest));
        }

        void TextArea::move_to_latest_highlight()
        {
            const auto highest = _selection_start < _selection_end ? _selection_end : _selection_start;
            highlight(highest, highest);
            move_visual_cursor_position(logical_to_visual(highest));
        }

        void TextArea::update_cursor()
        {
            // Get the current line and the text it is rendering.
            auto line = current_line();
            if (line == nullptr)
            {
                _cursor->set_visible(false);
                return;
            }

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

        TextArea::VisualPosition TextArea::logical_to_visual(TextArea::LogicalPosition point) const
        {
            if (_line_structure.empty())
            {
                return { 0u, 0u };
            }

            const auto iter = std::find_if(_line_structure.begin(), _line_structure.end(), [&](const auto& entry)
                {
                    return point.line == entry.line && point.position >= entry.start && point.position <= entry.start + entry.length;
                });
            if (iter == _line_structure.end())
            {
                return { static_cast<int32_t>(_line_structure.size() - 1), _line_structure.back().length };
            }
            return { static_cast<int32_t>(iter - _line_structure.begin()), point.position - iter->start };
        }

        TextArea::LogicalPosition TextArea::visual_to_logical(TextArea::VisualPosition point) const
        {
            if (_line_structure.empty())
            {
                return { 0, 0 };
            }

            if (point.line < _line_structure.size())
            {
                return { _line_structure[point.line].line, _line_structure[point.line].start + point.position };
            }
            return { static_cast<int32_t>(_line_structure.size()) - 1, _line_structure.back().length };
        }

        TextArea::VisualPosition TextArea::position_to_visual(const Point& position) const
        {
            const Point clamped(std::clamp(0.0f, position.x, size().width), 
                std::max(position.y, _lines[0]->position().y));

            int32_t matched_line = 0;
            for (const auto& child : _lines)
            {
                if (clamped.y >= child->position().y && clamped.y <= child->position().y + child->size().height)
                {
                    break;
                }
                ++matched_line;
            }

            int32_t index = std::min(matched_line, static_cast<int32_t>(_lines.size()) - 1) + _scroll_offset;
            return { index, find_nearest_index(index, clamped.x) };
        }

        void TextArea::delete_selection()
        {
            const auto earlier = _selection_start < _selection_end ? _selection_start : _selection_end;
            const auto later = _selection_start < _selection_end ? _selection_end : _selection_start;

            for (int i = later.line; i >= static_cast<int>(earlier.line); --i)
            {
                if (i >= _text.size())
                {
                    continue;
                }

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
                    if (i + 1 < _text.size())
                    {
                        _text.erase(_text.begin() + i + 1);
                    }
                }
                else
                {
                    _text.erase(_text.begin() + i);
                }
            }

            highlight(earlier, earlier);
            update_structure();
            move_visual_cursor_position(logical_to_visual(earlier));
            notify_text_updated();
        }

        bool TextArea::any_text_selected() const
        {
            return _selection_start != _selection_end;
        }

        std::wstring TextArea::selected_text() const
        {
            const auto earlier = _selection_start < _selection_end ? _selection_start : _selection_end;
            const auto later = _selection_start < _selection_end ? _selection_end : _selection_start;

            std::wstring output;
            for (auto i = earlier.line; i <= later.line; ++i)
            {
                if (i == earlier.line)
                {
                    if (i == later.line)
                    {
                        output = _text[i].substr(earlier.position, later.position);
                    }
                    else
                    {
                        output += _text[i].substr(earlier.position) + L'\n';
                    }
                }
                else if (i == later.line)
                {
                    output += _text[i].substr(0, later.position);
                }
                else
                {
                    output += _text[i] + L'\n';
                }
            }
            return output;
        }

        bool TextArea::read_only() const
        {
            return _read_only;
        }

        void TextArea::set_read_only(bool value)
        {
            _read_only = value;
        }
    }
}
