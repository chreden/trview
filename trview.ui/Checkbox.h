#pragma once

#include "Image.h"

namespace trview
{
    namespace ui
    {
        class Checkbox : public Image
        {
        public:
            Checkbox(Point position, Size size, graphics::Texture up_image, graphics::Texture down_image);
            virtual ~Checkbox() = default;

            // This event is raised when the user changes the state of the checkbox.
            // bool: The new state of the checkbox.
            Event<bool> on_state_changed;

            // Gets whether the checkbox is currently checked.
            // Returns: Whether the checkbox is currently checked.
            bool state() const;

            // Set the state of the checkbox. This will not raise the state changed event.
            // state: The new state of the checkbox.
            void set_state(bool state);
        protected:
            virtual bool clicked(Point position) override;
        private:
            graphics::Texture _up_image;
            graphics::Texture _down_image;
            bool    _state{ false };
        };
    }
}
