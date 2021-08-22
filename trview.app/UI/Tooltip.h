#pragma once

#include <string>
#include <trview.common/Point.h>
#include <trview.common/Colour.h>

namespace trview
{
    namespace ui
    {
        class Label;
        class Control;
    }

    /// Shows text in a floating control.
    class Tooltip final
    {
    public:
        /// Create a new tooltip.
        /// @param control The parent control.
        explicit Tooltip(ui::Control& control);

        /// Set the position of the tooltip. The tooltip will be offset so as not to cover the subject.
        /// @param position The new location of the tooltip.
        void set_position(const Point& position);

        /// Set the tooltip text.
        /// @param text The text to show.
        void set_text(const std::wstring& text);

        /// Set the colour of the text.
        /// @param colour The text colour.
        void set_text_colour(const Colour& colour);

        /// Set whether the tooltip is visible.
        /// @param value Whether the tooltip is visible.
        void set_visible(bool value);

        /// Gets whether the tooltip is visible.
        bool visible() const;
    private:
        ui::Control* _container;
        ui::Label* _label;
    };
}
