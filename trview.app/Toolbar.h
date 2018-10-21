/// @file Toolbar.h
/// @brief Window that contains buttons for speedrun planning tools.

#pragma once

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>

namespace trview
{
    /// Window that contains buttons for speedrun planning tools.
    class Toolbar final
    {
    public:
        /// Create a new Toolbar.
        /// @param parent The control to add the toolbar to.
        explicit Toolbar(ui::Control& parent);
    private:
        ui::StackPanel* _toolbar;
        TokenStore      _token_store;
    };
}
