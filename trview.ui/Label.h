#pragma once

#include <string>
#include <trview.common/Colour.h>

#include "Window.h"

#include <trview.graphics/TextAlignment.h>
#include <trview.graphics/ParagraphAlignment.h>
#include "SizeMode.h"

namespace trview
{
    namespace ui
    {
        class Label : public Window
        {
        public:
            Label(Point position, 
                Size size, 
                Colour background_colour, 
                std::wstring text, 
                float text_size = 20.0f,
                graphics::TextAlignment text_alignment = graphics::TextAlignment::Left,
                graphics::ParagraphAlignment paragraph_alignment = graphics::ParagraphAlignment::Near,
                SizeMode mode = SizeMode::Manual);

            virtual      ~Label() = default;
            std::wstring text() const;
            float        text_size() const;
            void         set_text(std::wstring text);
            graphics::TextAlignment      text_alignment() const;
            graphics::ParagraphAlignment paragraph_alignment() const;
            SizeMode size_mode() const;

            // Event raised when the text in the label has changed.
            Event<std::wstring> on_text_changed;

            /// Get the colour to use for the text.
            /// @returns The text colour.
            Colour text_colour() const;

            /// Set the text colour for the label.
            /// @param colour The new text colour.
            void set_text_colour(const Colour& colour);
        private:
            std::wstring                 _text;
            float                        _text_size;
            graphics::TextAlignment      _text_alignment;
            graphics::ParagraphAlignment _paragraph_alignment;
            SizeMode                     _size_mode;
            Colour                       _text_colour;
        };
    }
}