/// @file Checkbox.h
/// @brief UI element that provides a toggleable checkbox and an optional label.

#pragma once

#include <trview.graphics/Texture.h>

#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        class Image;
        class Label;

        /// UI element that provides a toggleable checkbox and an optional label.
        class Checkbox : public StackPanel
        {
        public:
            /// Creates a checkbox.
            /// @param position The position of the checkbox.
            /// @param size The size of the checkbox.
            /// @param up_image The texture to use when the checkbox isn't checked.
            /// @param down_image The texture to use when the checkbox is checked.
            Checkbox(const Point& position, 
                     const Size& size, 
                     const graphics::Texture& up_image, 
                     const graphics::Texture& down_image);

            /// Creates a checkox with an associated label.
            /// @param position The position of the checkbox.
            /// @param size The size of the checkbox.
            /// @param up_image The texture to use when the checkbox isn't checked.
            /// @param down_image The texture to use when the checkbox is checked.
            /// @param label_text The text for the associated label.
            Checkbox(const Point& position,
                     const Size& size,
                     const graphics::Texture& up_image,
                     const graphics::Texture& down_image,
                     const std::wstring& label_text);

            /// Destructor for the checkbox.
            virtual ~Checkbox() = default;

            /// This event is raised when the user changes the state of the checkbox.
            /// The new state of the checkbox is passed as a paramter to the listener.
            Event<bool> on_state_changed;

            /// Gets whether the checkbox is currently checked.
            /// @returns Whether the checkbox is currently checked.
            bool state() const;

            /// Set the state of the checkbox. This will not raise the state changed event.
            /// @param state The new state of the checkbox.
            void set_state(bool state);

            /// Set whether the checkbox is enabled.
            /// @param enabled Whether the checkbox is enabled.
            void set_enabled(bool enabled);
        protected:
            virtual bool clicked(Point position) override;
        private:
            void create_image(const Size& size);

            ui::Image* _image;
            ui::Label* _label;
            graphics::Texture _up_image;
            graphics::Texture _down_image;
            bool    _state{ false };
            bool    _enabled{ true };
        };
    }
}
