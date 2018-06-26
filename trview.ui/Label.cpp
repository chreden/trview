#include "Label.h"

namespace trview
{
    namespace ui
    {
        Label::Label(Point position, Size size, Colour background_colour, std::wstring text, float text_size, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment, SizeMode mode)
            : Window(position, size, background_colour), _text(text), _text_size(text_size), _text_alignment(text_alignment), _paragraph_alignment(paragraph_alignment), _size_mode(mode), _text_colour(1.0f, 1.0f, 1.0f, 1.0f)
        {
        }

        std::wstring Label::text() const
        {
            return _text;
        }

        float Label::text_size() const
        {
            return _text_size;
        }

        void Label::set_text(std::wstring text)
        {
            _text = text;
            on_text_changed(text);
            on_invalidate();
        }

        TextAlignment Label::text_alignment() const
        {
            return _text_alignment;
        }

        ParagraphAlignment Label::paragraph_alignment() const
        {
            return _paragraph_alignment;
        }

        SizeMode Label::size_mode() const
        {
            return _size_mode;
        }
 
        Colour Label::text_colour() const
        {
            return _text_colour;
        }

        void Label::set_text_colour(const Colour& colour)
        {
            _text_colour = colour;
            on_text_changed(_text);
            on_invalidate();
        }
    }
}