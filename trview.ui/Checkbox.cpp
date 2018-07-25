#include "Checkbox.h"
#include "Image.h"
#include "Label.h"

namespace trview
{
    using namespace graphics;

    namespace ui
    {
        Checkbox::Checkbox(const Point& position, const Size& size, const Texture& up_image, const Texture& down_image)
            : StackPanel(position, size, Colour(0.0f, 0.0f, 0.0f, 0.0f), Size(), Direction::Horizontal), _up_image(up_image), _down_image(down_image)
        {
            create_image(size);
            _image->set_texture(_up_image);
        }

        Checkbox::Checkbox(const Point& position, const Size& size, const Texture& up_image, const Texture& down_image, const std::wstring& label_text)
            : StackPanel(position, size, Colour(0.0f, 0.0f, 0.0f, 0.0f), Size(), Direction::Horizontal), _up_image(up_image), _down_image(down_image)
        {
            create_image(size);
            _image->set_texture(_up_image);

            add_child(std::make_unique<Window>(Point(), Size(3, size.height), Colour(1.0f, 0.5f, 0.5f, 0.5f)));

            auto label = std::make_unique<Label>(Point(), Size(1, 1), Colour(1.0f, 0.5f, 0.5f, 0.5f), label_text, 8, TextAlignment::Left, ParagraphAlignment::Centre, SizeMode::Auto);
            label->set_vertical_alignment(Align::Centre);
            _label = label.get();
            add_child(std::move(label));
        }

        void Checkbox::create_image(const Size& size)
        {
            auto image = std::make_unique<Image>(Point(), size);
            
            // Store local copies so that the image can be updated.
            _image = image.get();

            // Add the image and the label to the control.
            add_child(std::move(image));
        }

        bool Checkbox::clicked(Point)
        {
            set_state(!_state);
            on_state_changed(_state);
            return true;
        }

        // Gets whether the checkbox is currently checked.
        // Returns: Whether the checkbox is currently checked.
        bool Checkbox::state() const
        {
            return _state;
        }

        // Set the state of the checkbox. This will not raise the state changed event.
        // state: The new state of the checkbox.
        void Checkbox::set_state(bool state)
        {
            _state = state;
            _image->set_texture(_state ? _down_image : _up_image);
        }

        void Checkbox::set_enabled(bool enabled)
        {
            _enabled = enabled;
            set_handles_input(enabled);
            _label->set_text_colour(enabled ? Colour(1.0f, 1.0f, 1.0f, 1.0f) : Colour(1.0f, 0.7f, 0.7f, 0.7f));
        }
    }
}
