#pragma once

#include <trview.app/Tools/Toolbar.h>
#include <trview.app/UI/CameraControls.h>
#include <trview.app/UI/CameraPosition.h>
#include <trview.app/UI/Console.h>
#include <trview.app/UI/ContextMenu.h>
#include <trview.app/UI/GoTo.h>
#include <trview.app/UI/IViewerUI.h>
#include <trview.app/UI/LevelInfo.h>
#include <trview.app/UI/RoomNavigator.h>
#include <trview.app/UI/SettingsWindow.h>
#include <trview.app/UI/Tooltip.h>
#include <trview.app/UI/ViewOptions.h>
#include <trview.input/Mouse.h>
#include <trview.ui/Input.h>
#include <trview.ui.render/Renderer.h>
#include <trview.ui.render/MapRenderer.h>

namespace trview
{
    struct IShortcuts;

    class ViewerUI final : public IViewerUI
    {
    public:
        explicit ViewerUI(const Window& window,
            const std::shared_ptr<ITextureStorage>& texture_storage,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const ui::render::IRenderer::RendererSource& ui_renderer_source,
            const ui::render::IMapRenderer::MapRendererSource& map_renderer_source);

        virtual ~ViewerUI() = default;

        /// Clear the highlighted minimap sector.
        virtual void clear_minimap_highlight() override;

        /// Get the currently hovered minimap sector, if any.
        virtual std::shared_ptr<Sector> current_minimap_sector() const override;

        /// Get whether there is any text input currently active.
        virtual bool is_input_active() const override;

        /// Determiens if the cursor is over any ui element.
        /// @returns Whether the cursor is over an element.
        virtual bool is_cursor_over() const override;

        /// Render the UI.
        /// @param device The device to use to render the UI.
        virtual void render(const graphics::IDevice& device) override;

        /// Set whether an alternate group is enabled.
        /// @param value The group to change.
        /// @param enabled Whether the group is enabled.
        virtual void set_alternate_group(uint32_t value, bool enabled) override;

        /// Set the alternate groups for the level.
        /// @param groups The alternate groups for the level.
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) override;

        /// Set the current camera position.
        /// @param position The camera position.
        virtual void set_camera_position(const DirectX::SimpleMath::Vector3& position) override;

        /// Set the camera mode.
        /// @param mode The current camera mode.
        virtual void set_camera_mode(CameraMode mode) override;

        /// Set the camera projection mode.
        /// @param mode The current camera projection mode.
        virtual void set_camera_projection_mode(ProjectionMode mode) override;

        /// Set whether depth is enabled.
        /// @param value Whether depth is enabled.
        virtual void set_depth_enabled(bool value) override;

        /// Set the level of depth.
        /// @param value The depth level.
        virtual void set_depth_level(int32_t value) override;

        /// Set the current flip state.
        /// @param value The flip state.
        virtual void set_flip(bool value) override;

        /// Set whether there are any flipmaps in the level.
        /// @param value Whether there are any flipmaps.
        virtual void set_flip_enabled(bool value) override;

        /// Set whether the hide button on the context menu is enabled.
        /// @param value Whether the hide button is enabled.
        virtual void set_hide_enabled(bool value) override;

        /// Set whether highlight is enabled.
        /// @param value Whether highlight is enabled.
        virtual void set_highlight(bool value) override;

        /// Set the size of the host window.
        void set_host_size(const Size& size) override;

        /// Set the level name and version.
        /// @param name The file.
        /// @param version The version of the level.
        virtual void set_level(const std::string& name, trlevel::LevelVersion version) override;

        /// Set the maximum number of rooms in the level.
        /// @param rooms The number of rooms that are in the level.
        virtual void set_max_rooms(uint32_t rooms) override;

        /// Set the current measure distance to display on the label.
        /// @param distance The distance to measure.
        virtual void set_measure_distance(float distance) override;

        /// Set the position of the measure label.
        /// @param position The position of the label.
        virtual void set_measure_position(const Point& position) override;

        /// Set which square is highlighted on the minimap.
        /// @param x The x coordinate.
        /// @param z The z coordinate.
        virtual void set_minimap_highlight(uint16_t x, uint16_t z) override;

        /// Set the current pick result.
        /// @param info The parameters for the pick.
        /// @param pick_result The pick result.
        virtual void set_pick(const PickInfo& info, const PickResult& pick_result) override;

        /// Set whether the user can click the remove waypoint button.
        /// "param value Whether the button is enabled.
        virtual void set_remove_waypoint_enabled(bool value) override;

        /// Set the selected room.
        /// @param room The selected room.
        virtual void set_selected_room(Room* room) override;

        /// Set the user settings.
        /// @param settings The user settings.
        virtual void set_settings(const UserSettings& settings) override;

        /// Set whether the context menu is visible.
        /// "param value Whether the context menu is visible.
        virtual void set_show_context_menu(bool value) override;

        /// Set whether hidden geometry is visible.
        /// @param value Whether hidden geometry is visible.
        virtual void set_show_hidden_geometry(bool value) override;

        /// Set whether to show the measure label.
        /// @param value Whether to show the measure label.
        virtual void set_show_measure(bool value) override;

        /// Set whether to show the minimap.
        /// @param value Whether to show the minimap.
        virtual void set_show_minimap(bool value) override;

        /// Set whether to show the tooltip.
        /// @param value Whether to show the tooltip.
        virtual void set_show_tooltip(bool value) override;

        /// Set whether triggers are visible.
        /// @param value Whether triggers are visible.
        virtual void set_show_triggers(bool value) override;

        /// Set whether water is visible.
        /// @param value Whether water is visible.
        virtual void set_show_water(bool value) override;

        /// Set whether wireframe is visible.
        /// @param value Whether wireframe is visible.
        virtual void set_show_wireframe(bool value) override;

        /// Set whether the level uses alternate groups.
        /// @param value Whether alternate groups are used.
        virtual void set_use_alternate_groups(bool value) override;

        /// Set whether the UI is visible.
        /// @param value Whether the UI is visible.
        virtual void set_visible(bool value) override;

        /// Get whether hidden geometry is visible.
        virtual bool show_hidden_geometry() const override;

        /// Get whether triggers are visible.
        virtual bool show_triggers() const override;

        /// Get whether water is visible.
        virtual bool show_water() const override;

        /// Get whether wireframe is visible.
        virtual bool show_wireframe() const override;

        /// Get whether the context menu is visible.
        virtual bool show_context_menu() const override;

        /// Toggle the visibility of the settings window.
        virtual void toggle_settings_visibility() override;

        /// <summary>
        /// Write the text to the console.
        /// </summary>
        /// <param name="text">The text to write.</param>
        virtual void print_console(const std::wstring& text) override;

        virtual void initialise_input() override;
    private:
        void generate_tool_window(const ITextureStorage& texture_storage);
        void initialise_camera_controls(ui::Control& parent);
        void register_change_detection(ui::Control* control);

        TokenStore _token_store;
        input::Mouse _mouse;
        Window _window;
        UserSettings _settings;
        std::shared_ptr<IShortcuts> _shortcuts;
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::IRenderer> _ui_renderer;
        std::unique_ptr<ui::Input> _ui_input;
        std::unique_ptr<ContextMenu> _context_menu;
        std::unique_ptr<GoTo> _go_to;
        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<ViewOptions> _view_options;
        std::unique_ptr<Toolbar> _toolbar;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<SettingsWindow> _settings_window;
        std::unique_ptr<CameraControls> _camera_controls;
        std::unique_ptr<CameraPosition> _camera_position;
        std::unique_ptr<ui::render::IMapRenderer> _map_renderer;
        std::unique_ptr<Tooltip> _map_tooltip;
        std::unique_ptr<Tooltip> _tooltip;
        std::unique_ptr<Console> _console;
        ui::Label* _measure;
        bool _show_tooltip{ true };
    };
}
