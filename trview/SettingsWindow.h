/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

namespace trview
{
    namespace ui
    {
        class Control;
    }

    /// UI window for program level settings.
    class SettingsWindow final
    {
    public:
        /// Creates an instance of the SettingsWindow class. This will add UI elements to the control provided.
        /// @param parent The control to which the instance will add elements.
        explicit SettingsWindow(ui::Control& parent);
    private:

    };
}
