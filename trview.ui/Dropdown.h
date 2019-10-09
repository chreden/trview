#pragma once

#include <vector>
#include <string>

#include "Window.h"
#include "Button.h"

namespace trview
{
    namespace ui
    {
        class Listbox;

        class Dropdown final : public Window
        {
        public:
            /// A value element for the dropdown.
            struct Value
            {
                std::wstring text;
                Colour       foreground;
                Colour       background;
            };

            /// Create a new dropdown box.
            /// @param position The position of the control.
            /// @param size The size of the control.
            explicit Dropdown(const Point& position, const Size& size);

            /// Destructor for Dropdown.
            virtual ~Dropdown() = default;

            /// Set the scope that the dropdown will appear in. This may be required if the parent control
            /// does not have space for the entire dropdown to be rendered.
            /// @param scope The scope to create the dropdown control in.
            void set_dropdown_scope(Control* scope);

            /// Set the values to show in the dropdown.
            /// @param value_names The values to show.
            void set_values(const std::vector<std::wstring>& value_names);

            /// Set the values to show in the dropdown.
            /// @param values The values to show.
            void set_values(const std::vector<Value>& values);

            /// Set the selected value. This will not raise the on_value_selected event.
            /// @param value The new value.
            void set_selected_value(const std::wstring& value);

            /// Set the background colour of the selected text box.
            /// @param colour The new value.
            void set_text_background_colour(const Colour& colour);

            /// Event raised when a value is selected. The selected value is passed as a parameter.
            Event<std::wstring> on_value_selected;

            virtual void lost_focus(Control* new_focus) override;
        private:
            void update_dropdown();

            std::vector<Value> _values;
            ui::Button* _button;
            ui::Listbox* _dropdown;
        };
    }
}
