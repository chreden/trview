/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class Control;
    }
    struct ITextureStorage;

    /// UI window for program level settings.
    class SettingsWindow final
    {
    public:
        /// Creates an instance of the SettingsWindow class. This will add UI elements to the control provided.
        /// @param parent The control to which the instance will add elements.
        /// @param texture_storage The texture storage instance to use.
        explicit SettingsWindow(ui::Control& parent, const ITextureStorage& texture_storage);

        /// Event raised when the vsync settings has been changed. The new vsync setting is passed as the parameter.
        Event<bool> on_vsync;

        /// Event raised when the 'go to lara' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_go_to_lara;
    };
}
