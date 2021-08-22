/// @file Toolbar.h
/// @brief Window that contains buttons for speedrun planning tools.

#pragma once

#include <string>
#include <trview.ui/Control.h>

namespace trview
{
    /// Window that contains buttons for speedrun planning tools.
    class Toolbar final
    {
    public:
        /// Create a new Toolbar.
        /// @param parent The control to add the toolbar to.
        explicit Toolbar(ui::Control& parent);

        /// Add a new tool to the toolbar.
        /// @param name The name of the tool
        /// @param text The text to show in the button.
        void add_tool(const std::wstring& name, const std::wstring& text);

        /// Event raised when a tool button is clicked.
        /// @remarks The name of the tool is passed as a parameter.
        Event<const std::wstring&> on_tool_clicked;
    private:
        ui::Control* _toolbar;
        TokenStore _token_store;
    };
}
