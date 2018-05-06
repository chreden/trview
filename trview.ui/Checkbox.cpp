#include "Checkbox.h"

namespace trview
{
    namespace ui
    {
        Checkbox::Checkbox(Point position, Size size, Texture up_image, Texture down_image)
            : Image(position, size), _up_image(up_image), _down_image(down_image)
        {
            set_texture(_up_image);
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
            set_texture(_state ? _down_image : _up_image);
        }
    }
}
