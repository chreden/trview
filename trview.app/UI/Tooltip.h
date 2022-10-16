#pragma once

#include <string>
#include <trview.common/Colour.h>

namespace trview
{
    /// Shows text in a floating control.
    class Tooltip final
    {
    public:
        /// Set the tooltip text.
        /// @param text The text to show.
        void set_text(const std::string& text);

        /// Set the colour of the text.
        /// @param colour The text colour.
        void set_text_colour(const Colour& colour);

        /// Set whether the tooltip is visible.
        /// @param value Whether the tooltip is visible.
        void set_visible(bool value);

        /// Gets whether the tooltip is visible.
        bool visible() const;

        void render();
    private:
        bool _visible{ false };
        std::string _text;
        Colour _colour{ Colour::White };
    };
}
