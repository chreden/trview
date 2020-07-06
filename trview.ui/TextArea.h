#pragma once

#include "StackPanel.h"
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
        private:
            struct LineEntry
            {
                uint32_t line;
                uint32_t start;
                uint32_t length;
            };

            struct CursorPoint
            {
                uint32_t line{ 0u };
                uint32_t position{ 0u };

                bool operator==(const CursorPoint& other) const
                {
                    return line == other.line && position == other.position;
                }

                bool operator!=(const CursorPoint& other) const 
                {
                    return !(*this == other);
                }

                bool operator<(const CursorPoint& other) const
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
            void move_visual_cursor_position(uint32_t line, uint32_t position);
            uint32_t find_nearest_index(uint32_t line, float x) const;
            void new_line();
            void highlight(CursorPoint start, CursorPoint end);
            void move_to_earliest_highlight();
            void move_to_latest_highlight();
            CursorPoint logical_to_visual(CursorPoint point) const;
            CursorPoint visual_to_logical(CursorPoint point) const;
            void delete_selection();

            StackPanel*         _area;
            std::vector<Label*> _lines;
            Colour              _text_colour;
            Window*             _cursor;
            Mode                _mode{ Mode::MultiLine };
            bool                _focused{ false };
            graphics::TextAlignment _alignment{ graphics::TextAlignment::Left };
            std::vector<std::wstring> _text;
            std::vector<LineEntry>    _line_structure;
            CursorPoint               _visual_cursor{ 0u, 0u };
            CursorPoint               _logical_cursor{ 0u, 0u };
            // The first pin point of the selection. Not necessarily earlier in the text.
            CursorPoint               _selection_start;
            // The second pin point of the selection. Not necessarily later in the text.
            CursorPoint               _selection_end;
        };
    }
}
