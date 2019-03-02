#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Window.h>
#include <trview.ui/Control.h>
#include <trview.ui.render/Renderer.h>
#include <trview.input/Mouse.h>
#include <trview.ui.render/MapRenderer.h>

#include "Toolbar.h"
#include "LevelInfo.h"
#include "RoomNavigator.h"
#include "GoToRoom.h"
#include "SettingsWindow.h"
#include "Flipmaps.h"
#include "Neighbours.h"
#include "CameraControls.h"
#include "CameraPosition.h"
#include "PickInfo.h"
#include "PickResult.h"

namespace trview
{
    class ContextMenu;
    struct ILevelTextureStorage;

    namespace graphics
    {
        struct IShaderStorage;
        class FontFactory;
    }

    enum class Tool
    {
        None,
        Measure
    };

    class ViewerUI final
    {
    public:
        explicit ViewerUI(const Window& window, 
            const graphics::Device& device, 
            const graphics::IShaderStorage& shader_storage,
            const graphics::FontFactory& font_factory,
            const ITextureStorage& texture_storage);

        /// Get the currently hovered minimap sector, if any.
        std::shared_ptr<Sector> current_minimap_sector() const;

        /// Determiens if the cursor is over any ui element.
        /// @returns Whether the cursor is over an element.
        bool is_cursor_over() const;

        /// Load the minimap from the specified room.
        void load_minimap(trview::Room* room);

        /// Event raised when an alternate group is toggled.
        Event<uint16_t, bool> on_alternate_group;

        /// Event raised when the camera mode is set.
        Event<CameraMode> on_camera_mode;

        /// Event raised when the camera is reset.
        Event<> on_camera_reset;

        /// Event raised when the camera sensitivity is changed.
        Event<float> on_camera_sensitivity;

        /// Event raised when the camera movement speed is changed.
        Event<float> on_camera_movement_speed;

        /// Event raised when the depth level changes.
        Event<int32_t> on_depth_level_changed;

        /// Event raised when the depth setting is changed.
        Event<bool> on_depth;

        /// Event raised when the flip settings is changed.
        Event<bool> on_flip;

        /// Event raised when the higlight setting is changed.
        Event<bool> on_highlight;

        /// Event raised when a room is selected.
        Event<int32_t> on_select_room;

        /// Event raised when the hidden geometry setting is changed.
        Event<bool> on_show_hidden_geometry;

        /// Event raised when the show triggers setting is changed.
        Event<bool> on_show_triggers;

        /// Event raised when the show water setting is changed.
        Event<bool> on_show_water;

        /// Event raised when a tool is selected.
        Event<Tool> on_tool_selected;

        /// Render the UI.
        /// @param device The device to use to render the UI.
        void render(const graphics::Device& device);

        /// Set whether an alternate group is enabled.
        /// @param value The group to change.
        /// @param enabled Whether the group is enabled.
        void set_alternate_group(uint16_t value, bool enabled);

        /// Set the alternate groups for the level.
        /// @param groups The alternate groups for the level.
        void set_alternate_groups(const std::set<uint16_t>& groups);

        /// Set the camera movement speed.
        /// @param value The camera movement speed.
        void set_camera_movement_speed(float value);

        /// Set the current camera position.
        /// @param position The camera position.
        void set_camera_position(const DirectX::SimpleMath::Vector3& position);

        /// Set the sensitivity of the camera.
        /// @param value The camera sensitivity.
        void set_camera_sensitivity(float value);

        /// Set the camera mode.
        /// @param mode The current camera mode.
        void set_camera_mode(CameraMode mode);

        /// Set whether depth is enabled.
        /// @param value Whether depth is enabled.
        void set_depth_enabled(bool value);

        /// Set the level of depth.
        /// @param value The depth level.
        void set_depth_level(int32_t value);

        /// Set the current flip state.
        /// @param value The flip state.
        void set_flip(bool value);

        /// Set whether there are any flipmaps in the level.
        /// @param value Whether there are any flipmaps.
        void set_flip_enabled(bool value);

        /// Set whether highlight is enabled.
        /// @param value Whether highlight is enabled.
        void set_highlight(bool value);

        /// Set the size of the host window.
        void set_host_size(const Size& size);

        /// Set the level name and version.
        /// @param name The filename.
        /// @param version The version of the level.
        void set_level(const std::string& name, trlevel::LevelVersion version);

        /// Set the current pick result.
        /// @param info The parameters for the pick.
        /// @param pick_result The pick result.
        void set_pick(const PickInfo& info, const PickResult& pick_result);

        /// Set whether hidden geometry is visible.
        /// @param value Whether hidden geometry is visible.
        void set_show_hidden_geometry(bool value);

        /// Set whether to show the tooltip.
        /// @param value Whether to show the tooltip.
        void set_show_tooltip(bool value);

        /// Set whether triggers are visible.
        /// @param value Whether triggers are visible.
        void set_show_triggers(bool value);

        /// Set whether water is visible.
        /// @param value Whether water is visible.
        void set_show_water(bool value);

        /// Set whether the level uses alternate groups.
        /// @param value Whether alternate groups are used.
        void set_use_alternate_groups(bool value);

        /// Set whether the UI is visible.
        /// @param value Whether the UI is visible.
        void set_visible(bool value);

        /// Get whether hidden geometry is visible.
        bool show_hidden_geometry() const;

        /// Get whether triggers are visible.
        bool show_triggers() const;

        /// Get whether water is visible.
        bool show_water() const;

        /// Toggle the visibility of the settings window.
        void toggle_settings_visibility();
    private:
        void generate_tool_window(const ITextureStorage& texture_storage);
        void initialise_camera_controls(ui::Control& parent);

        TokenStore _token_store;
        input::Mouse _mouse;

        Window _window;
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;
        std::unique_ptr<ContextMenu> _context_menu;
        std::unique_ptr<GoToRoom> _go_to_room;
        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<Toolbar> _toolbar;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<SettingsWindow> _settings_window;
        std::unique_ptr<Flipmaps> _flipmaps;
        std::unique_ptr<Neighbours> _neighbours;
        std::unique_ptr<CameraControls> _camera_controls;
        std::unique_ptr<CameraPosition> _camera_position;
        std::unique_ptr<ui::render::MapRenderer> _map_renderer;
        ui::Label* _tooltip;
        bool _show_tooltip{ true };
    };
}
