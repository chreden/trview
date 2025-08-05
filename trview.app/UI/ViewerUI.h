#pragma once

#include <trview.app/Tools/IToolbar.h>
#include <trview.app/UI/ICameraControls.h>
#include <trview.app/UI/CameraPosition.h>
#include <trview.app/UI/IContextMenu.h>
#include <trview.app/UI/GoTo.h>
#include <trview.app/UI/IViewerUI.h>
#include <trview.app/UI/ILevelInfo.h>
#include <trview.app/UI/RoomNavigator.h>
#include <trview.app/UI/ISettingsWindow.h>
#include <trview.app/UI/Tooltip.h>
#include <trview.app/UI/IViewOptions.h>
#include <trview.input/Mouse.h>
#include <trview.app/UI/IMapRenderer.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    class ViewerUI final : public IViewerUI
    {
    public:
        explicit ViewerUI(const Window& window,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const IMapRenderer::Source& map_renderer_source,
            std::unique_ptr<ISettingsWindow> settings_window,
            std::unique_ptr<IViewOptions> view_options,
            std::unique_ptr<IContextMenu> context_menu,
            std::unique_ptr<ICameraControls> camera_controls,
            std::unique_ptr<IToolbar> toolbar,
            std::unique_ptr<ILevelInfo> level_info);
        virtual ~ViewerUI() = default;
        virtual void clear_minimap_highlight() override;
        virtual std::shared_ptr<ISector> current_minimap_sector() const override;
        virtual bool is_input_active() const override;
        virtual bool is_cursor_over() const override;
        virtual void render() override;
        virtual void set_alternate_group(uint32_t value, bool enabled) override;
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) override;
        virtual void set_camera_position(const DirectX::SimpleMath::Vector3& position) override;
        virtual void set_camera_rotation(float yaw, float pitch) override;
        virtual void set_camera_mode(ICamera::Mode mode) override;
        virtual void set_camera_projection_mode(ProjectionMode mode) override;
        virtual void set_flip_enabled(bool value) override;
        virtual void set_hide_enabled(bool value) override;

        /// Set the size of the host window.
        void set_host_size(const Size& size) override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        virtual void set_max_rooms(uint32_t rooms) override;
        virtual void set_measure_distance(float distance) override;
        virtual void set_measure_position(const Point& position) override;
        virtual void set_minimap_highlight(uint16_t x, uint16_t z) override;
        virtual void set_pick(const PickResult& pick_result) override;
        virtual void set_remove_waypoint_enabled(bool value) override;
        virtual void set_selected_item(uint32_t index) override;
        virtual void set_selected_room(const std::shared_ptr<IRoom>& room) override;
        virtual void set_settings(const UserSettings& settings) override;
        virtual void set_show_context_menu(bool value) override;
        virtual void set_show_measure(bool value) override;
        virtual void set_show_minimap(bool value) override;
        virtual void set_show_tooltip(bool value) override;
        virtual void set_use_alternate_groups(bool value) override;
        virtual void set_visible(bool value) override;
        virtual bool show_context_menu() const override;
        virtual void toggle_settings_visibility() override;
        virtual void set_mid_waypoint_enabled(bool value) override;
        virtual void set_scalar(const std::string& name, int32_t value) override;
        virtual void set_toggle(const std::string& name, bool value) override;
        virtual bool toggle(const std::string& name) const override;
        virtual void set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        void set_route(const std::weak_ptr<IRoute>& route) override;
        void set_show_camera_position(bool value) override;
        void reset_layout() override;
        void set_tile_filter_enabled(bool value) override;
    private:
        void generate_tool_window();
        void render_route_notes();
        void render_scriptable_notes();

        TokenStore _token_store;
        input::Mouse _mouse;
        Window _window;
        UserSettings _settings;
        std::unique_ptr<IContextMenu> _context_menu;
        std::unique_ptr<GoTo> _go_to;
        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<IViewOptions> _view_options;
        std::unique_ptr<IToolbar> _toolbar;
        std::unique_ptr<ILevelInfo> _level_info;
        std::unique_ptr<ISettingsWindow> _settings_window;
        std::unique_ptr<ICameraControls> _camera_controls;
        std::unique_ptr<CameraPosition> _camera_position;
        std::unique_ptr<IMapRenderer> _map_renderer;
        std::unique_ptr<Tooltip> _map_tooltip;
        std::unique_ptr<Tooltip> _tooltip;
        bool _show_tooltip{ true };
        bool _show_measure{ false };
        std::string _measure_text;
        Point _measure_position;
        bool _visible{ true };
        uint32_t _selected_room{ 0u };
        uint32_t _selected_item{ 0u };
        std::weak_ptr<IRoute> _route;
        std::weak_ptr<ILevel> _level;
    };
}
