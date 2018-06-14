#include "Button.h"
#include "Label.h"
#include "Image.h"

namespace trview
{
    namespace ui
    {
        Button::Button(Point position, Size size, graphics::Texture up_image, graphics::Texture down_image)
            : Control(position, size), _up_image(up_image), _down_image(down_image)
        {
            auto image = std::make_unique<Image>(Point(), size);
            image->set_texture(up_image);
            add_child(std::move(image));
        }

        Button::Button(Point position, Size size, const std::wstring& text)
            : Control(position, size)
        {
            add_child(std::make_unique<Label>(Point(2,2), size - Size(4,4), Colour(1.0f, 0.4f, 0.4f, 0.4f), text, 10.0f, TextAlignment::Centre, ParagraphAlignment::Centre));
        }

        bool Button::clicked(Point)
        {
            on_click();
            return true;
        }

        uint32_t Button::border_thickness() const
        {
            return _border_thickness;
        }

        void Button::set_border_thickness(uint32_t thickness)
        {
            _border_thickness = thickness;
        }
    }
}
