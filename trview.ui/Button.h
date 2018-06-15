/// @file Button.h
/// @brief UI element that provides a clickable area.

#pragma once

#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>

#include "Control.h"

namespace trview
{
    namespace ui
    {
        /// UI element that provides a clickable area.
        class Button : public Control
        {
        public:
            /// Creates a new button with provided textures.
            /// @param position The position of the new element.
            /// @param size The size of the new element.
            /// @param up_image The texture to use when the button is in the up state.
            /// @param down_image The texture to use when the button is in the down state.
            Button(Point position, Size size, graphics::Texture up_image, graphics::Texture down_image);

            /// Creates a new button with a label.
            /// @param position The position of the new element.
            /// @param size The size of the new element.
            /// @param text The text to display on the button.
            Button(Point position, Size size, const std::wstring& text);

            /// Destructor for the button.
            virtual ~Button() = default;

            /// Event raised when the user clicks on the button.
            Event<> on_click;

            /// Gets the thickness of the border that will be applied when the element is rendered.
            uint32_t border_thickness() const;

            /// Set the thickness of the border that will be applied when the element is rendered.
            /// @param thickness The new thickness.
            void set_border_thickness(uint32_t thickness);
        protected:
            virtual bool clicked(Point position) override;
        private:
            graphics::Texture _up_image;
            graphics::Texture _down_image;
            uint32_t _border_thickness{ 1u };
        };
    }
}
