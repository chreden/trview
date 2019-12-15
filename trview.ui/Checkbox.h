/// @file Checkbox.h
/// @brief UI element that provides a toggleable checkbox and an optional label.

#pragma once

#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        class Label;

        /// UI element that provides a toggleable checkbox and an optional label.
        class Checkbox : public StackPanel
        {
        public:
            /// Creates a checkbox.
            explicit Checkbox();

            /// Creates a checkbox.
            /// @param position The position of the checkbox.
            Checkbox(const Point& position);

            /// Creates a checkox with an associated label.
            /// @param position The position of the checkbox.
            /// @param label_text The text for the associated label.
            Checkbox(const Point& position, const std::wstring& label_text);

            /// Creates a checkox with an associated label.
            /// @param position The position of the checkbox.
            /// @param background_colour Background colour of the label.
            /// @param label_text The text for the associated label.
            Checkbox(const Point& position, const Colour& background_colour, const std::wstring& label_text);

            /// Creates a checkbox with an associated label.
            /// @param background_colour Background colour of the label.
            /// @param label_text The text for the associated label.
            Checkbox(const Colour& background_colour, const std::wstring& label_text);

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

            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool clicked(Point position) override;
        private:
            void create_image(const Size& size);

            ui::Label*  _label;
            ui::Label*  _check;
            bool        _state{ false };
            bool        _enabled{ true };
        };
    }
}
