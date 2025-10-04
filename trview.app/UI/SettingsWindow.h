/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

#include "ISettingsWindow.h"

#include <trview.common/Windows/IShell.h>
#include <trview.common/Windows/IDialogs.h>
#include "Fonts/IFonts.h"

namespace trview
{
    struct ITextureStorage;

    /// UI window for program level settings.
    class SettingsWindow final : public ISettingsWindow
    {
    public:
        struct Names
        {
            static inline const std::string vsync = "Vsync";
            static inline const std::string go_to_lara = "Select Lara when level is opened";
            static inline const std::string invert_map_controls = "Invert map controls";
            static inline const std::string items_startup = "Open Items Window at startup";
            static inline const std::string triggers_startup = "Open Triggers Window at startup";
            static inline const std::string rooms_startup = "Open Rooms Window at startup";
            static inline const std::string auto_orbit = "Switch to orbit on selection";
            static inline const std::string invert_vertical_pan = "Invert vertical panning";
            static inline const std::string camera_display_degrees = "Use degrees for camera angle display";
            static inline const std::string randomizer_tools = "Enable Randomizer Tools";
            static inline const std::string max_recent_files = "Recent Files";
            static inline const std::string sensitivity = "Sensitivity";
            static inline const std::string movement_speed = "Movement Speed";
            static inline const std::string acceleration = "Acceleration";
            static inline const std::string acceleration_rate = "Acceleration Rate";
            static inline const std::string background_colour = "Background Colour";
            static inline const std::string default_route_colour = "Default Route Colour";
            static inline const std::string default_waypoint_colour = "Default Waypoint Colour";
            static inline const std::string route_startup = "Open Route Window at startup";
            static inline const std::string fov = "Camera FOV";
            static inline const std::string camera_sink_startup = "Open Camera/Sink Window at startup";
            static inline const std::string reset_fov = "Reset##Fov";
            static inline const std::string statics_startup = "Open Statics Window at startup";
            static inline const std::string linear_filtering = "Linear Filtering";
        };

        explicit SettingsWindow(const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IShell>& shell,
            const std::shared_ptr<IFonts>& fonts,
            const std::shared_ptr<ITextureStorage>& texture_storage);
        virtual ~SettingsWindow() = default;
        virtual void render() override;
        virtual void toggle_visibility() override;
        void set_settings(const UserSettings& settings) override;
    private:
        void show_texture_filtering_window();

        void checkbox(const std::string& name, bool& setting);
        void slider(const std::string& name, float& value, float min, float max);

        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IShell> _shell;
        bool _visible{ false };
        UserSettings _settings;
        std::vector<FontSetting> _all_fonts;
        std::shared_ptr<IFonts> _fonts;
        bool _showing_filtering_popup{ false };
        graphics::Texture _linear_texture;
        graphics::Texture _point_texture;
    };
}
