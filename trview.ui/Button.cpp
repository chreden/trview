#include "stdafx.h"
#include "Button.h"
#include "Label.h"
#include "Image.h"

namespace trview
{
    namespace ui
    {
        namespace Colours
        {
            const Colour Background{ 0.25f, 0.25f, 0.25f };
            const Colour Highlight{ 0.0f, 0.05f, 0.05f, 0.05f };
        }

        Button::Button(Point position, Size size, graphics::Texture up_image, graphics::Texture down_image)
            : Window(position, size, Colour::Transparent), _up_image(up_image), _down_image(down_image), _text(nullptr)
        {
            auto image = std::make_unique<Image>(size);
            image->set_texture(up_image);
            add_child(std::move(image));
        }

        Button::Button(Point position, Size size, const std::wstring& text)
            : Window(position, size, Colours::Background)
        {
            _text = add_child(std::make_unique<Label>(Point(2, 2), size - Size(4, 4), Colours::Background, text, 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
            set_handles_hover(true);
        }

        Button::Button(Point position, Size size)
            : Window(position, size, Colours::Background), _text(nullptr)
        {
        }

        Button::Button(const Size& size)
            : Button(Point(), size)
        {
        }

        Button::Button(const Size& size, const std::wstring& text)
            : Button(Point(), size, text)
        {
        }

        Button::Button(const Size& size, graphics::Texture up_image, graphics::Texture down_image)
            : Button(Point(), size, up_image, down_image)
        {
        }

        bool Button::mouse_down(const Point&)
        {
            return true;
        }

        bool Button::mouse_up(const Point&)
        {
            return true;
        }

        void Button::mouse_enter()
        {
            if (_text)
            {
                // Store the old background colour so that if the background has been changed by a call
                // to set_background_colour between enter and leave we don't reset it to the wrong colour.
                _previous_colour = _text->background_colour();
                _text->set_background_colour(_previous_colour + Colours::Highlight);
            }
        }

        void Button::mouse_leave()
        {
            // Check that the button has the same background colour as when we changed to the highlight colour.
            if (_text && _text->background_colour() == _previous_colour + Colours::Highlight)
            {
                _text->set_background_colour(_previous_colour);
            }
        }

        bool Button::clicked(Point)
        {
            on_click();
            return true;
        }

        float Button::border_thickness() const
        {
            return _border_thickness;
        }

        void Button::set_border_thickness(float thickness)
        {
            _border_thickness = thickness;
            on_invalidate();
        }

        /// Set the text of the button, if it has a label for text.
        /// @param text The new text for the button.
        void Button::set_text(const std::wstring& text)
        {
            if (_text)
            {
                _text->set_text(text);
            }
        }

        void Button::set_text_background_colour(const Colour& colour)
        {
            if (_text)
            {
                _text->set_background_colour(colour);
            }
        }

        void Button::set_text_colour(const Colour& colour)
        {
            if (_text)
            {
                _text->set_text_colour(colour);
            }
        }
    }
}
