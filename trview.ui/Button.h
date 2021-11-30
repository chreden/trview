/// @file Button.h
/// @brief UI element that provides a clickable area.

#pragma once

#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>
#include <trview.common/Colour.h>

#include "Window.h"

#include <optional>

namespace trview
{
    struct Colour;

    namespace ui
    {
        class Label;
        class Image;

        /// UI element that provides a clickable area.
        class Button : public Window
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

            /// Creates a new button.
            /// @param size The size of the new element.
            explicit Button(const Size& size);

            /// Creates a new button with a label.
            /// @param size The size of the new element.
            /// @param text The text to display on the button.
            Button(const Size& size, const std::wstring& text);

            /// Creates a new button with provided textures.
            /// @param size The size of the new element.
            /// @param up_image The texture to use when the button is in the up state.
            /// @param down_image The texture to use when the button is in the down state.
            Button(const Size& size, graphics::Texture up_image, graphics::Texture down_image);

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

            void set_images(const graphics::Texture& up, const graphics::Texture& down);

            void set_font(const std::string& font);

            /// Get the text if set.
            /// @returns The text.
            std::wstring text() const;

            /// Get the text colour, if set.
            /// @returns The text colour.
            std::optional<Colour> text_colour() const;

            /// <summary>
            /// Get the text background colour, if set.
            /// </summary>
            std::optional<Colour> text_background_colour() const;

            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual void mouse_enter() override;
            virtual void mouse_leave() override;
            virtual bool clicked(Point position) override;
        private:
            graphics::Texture _up_image;
            graphics::Texture _down_image;
            float _border_thickness{ 1.0f };
            Label* _text;
            Colour _previous_colour;
            Image* _image{ nullptr };
        };
    }
}
