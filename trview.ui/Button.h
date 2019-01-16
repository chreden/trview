/// @file Button.h
/// @brief UI element that provides a clickable area.

#pragma once

#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>

#include "Control.h"

namespace trview
{
    struct Colour;

    namespace ui
    {
        class Label;

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

            /// Creates a new button.
            /// @param positio The position of the new element.
            /// @param size The size of the new element.
            Button(Point position, Size size);

            /// Destructor for the button.
            virtual ~Button() = default;

            /// Event raised when the user clicks on the button.
            Event<> on_click;

            /// Gets the thickness of the border that will be applied when the element is rendered.
            /// @returns The thickness of the border.s
            float border_thickness() const;

            /// Set the thickness of the border that will be applied when the element is rendered.
            /// @param thickness The new thickness.
            void set_border_thickness(float thickness);

            /// Set the text of the button, if it has a label for text.
            /// @param text The new text for the button.
            void set_text(const std::wstring& text);

            /// Set the background colour of the text, if present.
            /// @param colour The background colour.
            void set_text_background_colour(const Colour& colour);

            /// Set the foreground colour of the text, if present.
            /// @param colour The foreground colour.
            void set_text_colour(const Colour& colour);
        protected:
            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool clicked(Point position) override;
        private:
            graphics::Texture _up_image;
            graphics::Texture _down_image;
            float _border_thickness{ 1.0f };
            Label* _text;
        };
    }
}
