#pragma once

#include <vector>
#include <string>

#include "Window.h"
#include "Label.h"

namespace trview
{
    namespace ui
    {
        class StackPanel;

        class Dropdown final : public Window
        {
        public:
            /// Create a new dropdown box.
            /// @param position The position of the control.
            /// @param size The size of the control.
            explicit Dropdown(const Point& position, const Size& size);

            /// Destructor for Dropdown.
            virtual ~Dropdown() = default;

            /// Set the scope that the dropdown will appear in. This may be required if the parent control
            /// does not have space for the entire dropdown to be rendered.
            /// @param scope The scope to create the dropdown control in.
            void set_dropdown_scope(ui::Control* scope);

            /// Set the values to show in the dropdown.
            /// @param values The values to show.
            void set_values(const std::vector<std::wstring>& values);

            /// Event raised when a value is selected. The selected value is passed as a parameter.
            Event<std::wstring> on_value_selected;
        protected:
            virtual bool mouse_down(const Point&) override;
            virtual bool mouse_up(const Point&) override;
            virtual bool clicked(Point position) override;
        private:
            void update_dropdown();

            std::vector<std::wstring> _values;
            ui::Label*                _label;
            ui::StackPanel*           _dropdown;
        };
    }
}
