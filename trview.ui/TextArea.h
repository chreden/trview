#pragma once

#include "StackPanel.h"
#include "Scrollbar.h"
#include <trview.graphics/TextAlignment.h>

namespace trview
{
    namespace ui
    {
        class Label;

        /// A text area is a number of lines of text.
        class TextArea final : public Window
        {
        public:
            enum class Mode
            {
                MultiLine,
                SingleLine
            };

            /// Create a text area.
            /// @param size The size of the control.
            /// @param background_colour The background colour of the text area.
            /// @param text_colour The text colour for the text area.
            /// @param text_alignment The text alignment of the label.
            TextArea(const Size& size, const Colour& background_colour, const Colour& text_colour, graphics::TextAlignment text_alignment = graphics::TextAlignment::Left);

            /// Create a text area.
            /// @param position The position to place the control.
            /// @param size The size of the control.
            /// @param background_colour The background colour of the text area.
            /// @param text_colour The text colour for the text area.
            /// @param text_alignment The text alignment of the label.
            explicit TextArea(const Point& position, const Size& size, const Colour& background_colour, const Colour& text_colour, graphics::TextAlignment text_alignment = graphics::TextAlignment::Left);

            /// Set the text in the text area to be the specified text.
            /// @param text The text to use.
            void set_text(const std::wstring& text);

            /// Set the line mode of the text area.
            /// @param mode The new mode.
            void set_mode(Mode mode);

            /// Get the text content of the text area.
            std::wstring text() const;

            /// Event raised when the text in the text area has changed.
            Event<std::wstring> on_text_changed;

            /// Event raised when the user has pressed the enter button in single line mode.
            Event<std::wstring> on_enter;

            /// Event raised when the user has pressed the escape button.
            Event<> on_escape;

            /// Event raised when the user has pressed the tab button in single line mode.
            Event<std::wstring> on_tab;

            virtual bool mouse_down(const Point& position) override;
            virtual bool key_down(uint16_t key, bool control_pressed, bool shift_pressed) override;
            virtual bool key_char(wchar_t character) override;
            virtual void gained_focus() override;
            virtual void lost_focus(Control*) override;
            virtual bool paste(const std::wstring& text) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool move(Point position) override;
            virtual bool copy(std::wstring& output) override;
            virtual bool cut(std::wstring& output) override;
            virtual bool clicked(Point position) override;
            virtual bool scroll(int delta) override;
            bool read_only() const;
            void set_read_only(bool value);
        private:
            struct LineEntry
            {
                uint32_t line;
                uint32_t start;
                uint32_t length;
            };

            struct LogicalPosition
            {
                uint32_t line{ 0u };
                uint32_t position{ 0u };

                bool operator==(const LogicalPosition& other) const
                {
                    return line == other.line && position == other.position;
                }

                bool operator!=(const LogicalPosition& other) const
                {
                    return !(*this == other);
                }

                bool operator<(const LogicalPosition& other) const
                {
                    if (line < other.line)
                    {
                        return true;
                    }
                    if (line == other.line)
                    {
                        return position < other.position;
                    }
                    return false;
                }
            };

            struct VisualPosition
            {
                uint32_t line{ 0u };
                uint32_t position{ 0u };

                bool operator==(const VisualPosition& other) const
                {
                    return line == other.line && position == other.position;
                }

                bool operator!=(const VisualPosition& other) const
                {
                    return !(*this == other);
                }

                bool operator<(const VisualPosition& other) const
                {
                    if (line < other.line)
                    {
                        return true;
                    }
                    if (line == other.line)
                    {
                        return position < other.position;
                    }
                    return false;
                }
            };

            Label* current_line();
            void update_structure();
            void update_cursor();
            void notify_text_updated();
            void move_visual_cursor_position(VisualPosition position);
            uint32_t find_nearest_index(uint32_t line, float x) const;
            void new_line();
            void clear_highlight();
            void highlight(LogicalPosition start, LogicalPosition end);
            void move_to_earliest_highlight();
            void move_to_latest_highlight();
            VisualPosition logical_to_visual(LogicalPosition point) const;
            LogicalPosition visual_to_logical(VisualPosition point) const;
            VisualPosition position_to_visual(const Point& position) const;
            void delete_selection();
            bool any_text_selected() const;
            std::wstring selected_text() const;
            std::wstring word_at_cursor(LogicalPosition point) const;

            StackPanel*         _area;
            std::vector<Label*> _lines;
            Colour              _text_colour;
            Window*             _cursor;
            Scrollbar*          _scrollbar;
            Mode                _mode{ Mode::MultiLine };
            bool                _focused{ false };
            graphics::TextAlignment _alignment{ graphics::TextAlignment::Left };
            std::vector<std::wstring> _text;
            std::vector<LineEntry>    _line_structure;
            VisualPosition _visual_cursor{ 0u, 0u };
            LogicalPosition _logical_cursor{ 0u, 0u };
            // The first pin point of the selection. Not necessarily earlier in the text.
            LogicalPosition _selection_start;
            // The second pin point of the selection. Not necessarily later in the text.
            LogicalPosition _selection_end;
            bool _dragging{ false };
            bool _read_only{ false };
            int32_t _scroll_offset{ 0u };
        };
    }
}
