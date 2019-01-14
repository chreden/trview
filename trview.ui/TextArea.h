#pragma once

#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        class Label;

        /// A text area is a number of lines of text.
        class TextArea final : public Window
        {
        public:
            /// Create a text area.
            /// @param position The position to place the control.
            /// @param size The size of the control.
            /// @param background_colour The background colour of the text area.
            /// @param text_colour The text colour for the text area.
            explicit TextArea(const Point& position, const Size& size, const Colour& background_colour, const Colour& text_colour);

            /// Set the text in the text area to be the specified text.
            /// @param text The text to use.
            void set_text(const std::wstring& text);

            /// Event raised when the text in the text area has changed.
            Event<std::wstring> on_text_changed;
        protected:
            virtual bool mouse_down(const Point& position) override;
            virtual bool key_down(uint16_t key) override;
            virtual bool key_char(wchar_t character) override;
        private:
            Label* current_line(bool raise_event = true);

            /// Add a new line to the text area with the specified content.
            /// @param text The optional content to use.
            void add_line(std::wstring text = std::wstring(), bool raise_event = true);

            void remove_line(bool raise_event = true);

            void remove_line(uint32_t line);

            /// Move the cursor element to be in the correct place.
            void update_cursor(bool raise_event = true);

            void notify_text_updated();

            StackPanel*         _area;
            std::vector<Label*> _lines;
            Colour              _text_colour;
            Window*             _cursor;
            uint32_t            _cursor_position{ 0u };
            uint32_t            _cursor_line{ 0u };
        };
    }
}
