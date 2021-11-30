#include "Label.h"

namespace trview
{
    namespace ui
    {
        Label::Label(Size size, Colour background_colour, std::wstring text, int text_size, graphics::TextAlignment text_alignment, graphics::ParagraphAlignment paragraph_alignment, SizeMode mode)
            : Label(Point(), size, background_colour, text, text_size, text_alignment, paragraph_alignment, mode)
        {
        }

        Label::Label(Point position, Size size, Colour background_colour, std::wstring text, int text_size, graphics::TextAlignment text_alignment, graphics::ParagraphAlignment paragraph_alignment, SizeMode mode)
            : Window(position, size, background_colour), _text(text), _text_size(text_size), _text_alignment(text_alignment), _paragraph_alignment(paragraph_alignment), _size_mode(mode), _text_colour(1.0f, 1.0f, 1.0f, 1.0f)
        {
        }

        std::wstring Label::text() const
        {
            return _text;
        }

        int Label::text_size() const
        {
            return _text_size;
        }

        void Label::set_text(const std::wstring& text)
        {
            if (text == _text)
            {
                return;
            }

            _text = text;
            on_text_changed(text);
            on_invalidate();
        }

        graphics::TextAlignment Label::text_alignment() const
        {
            return _text_alignment;
        }

        graphics::ParagraphAlignment Label::paragraph_alignment() const
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

        Size Label::measure_text(const std::wstring& text) const
        {
            if (!_measurer)
            {
                return Size();
            }
            return _measurer->measure(text);
        }

        void Label::set_measurer(IFontMeasurer* measurer)
        {
            _measurer = measurer;
        }

        bool Label::is_valid_character(wchar_t character) const
        {
            return !_measurer || _measurer->is_valid_character(character);
        }

        std::string Label::font() const
        {
            return _font;
        }

        void Label::set_font(const std::string& font)
        {
            _font = font;
        }
    }
}