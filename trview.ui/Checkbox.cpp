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

        bool Checkbox::clicked(Point position)
        {
            set_state(!_state);
            on_state_changed(_state);
            return true;
        }

        void Checkbox::set_state(bool state)
        {
            _state = state;
            set_texture(_state ? _down_image : _up_image);
        }
    }
}
