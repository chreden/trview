#include "Button.h"
#include "Label.h"
#include "Image.h"

namespace trview
{
    namespace ui
    {
        Button::Button(Point position, Size size, graphics::Texture up_image, graphics::Texture down_image)
            : Control(position, size), _up_image(up_image), _down_image(down_image), _text(nullptr)
        {
            auto image = std::make_unique<Image>(Point(), size);
            image->set_texture(up_image);
            add_child(std::move(image));
        }

        Button::Button(Point position, Size size, const std::wstring& text)
            : Control(position, size)
        {
            auto label = std::make_unique<Label>(Point(2, 2), size - Size(4, 4), Colour(1.0f, 0.4f, 0.4f, 0.4f), text, 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
            _text = label.get();
            add_child(std::move(label));
        }

        Button::Button(Point position, Size size)
            : Control(position, size), _text(nullptr)
        {
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
    }
}
