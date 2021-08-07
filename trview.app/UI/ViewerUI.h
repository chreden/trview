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
#include <trview.app/UI/ISettingsWindow.h>
#include <trview.app/UI/Tooltip.h>
#include <trview.app/UI/IViewOptions.h>
#include <trview.input/Mouse.h>
#include <trview.ui/Input.h>
#include <trview.ui.render/Renderer.h>
#include <trview.ui.render/MapRenderer.h>

namespace trview
{
    class ViewerUI final : public IViewerUI
    {
    public:
        explicit ViewerUI(const Window& window,
            const std::shared_ptr<ITextureStorage>& texture_storage,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const ui::IInput::Source& input_source,
            const ui::render::IRenderer::Source& ui_renderer_source,
            const ui::render::IMapRenderer::Source& map_renderer_source,
            const ISettingsWindow::Source& settings_window_source,
            const IViewOptions::Source& view_options_source);
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
        virtual void set_camera_mode(CameraMode mode) override;
        virtual void set_camera_projection_mode(ProjectionMode mode) override;
        virtual void set_depth_enabled(bool value) override;
        virtual void set_depth_level(int32_t value) override;
        virtual void set_flip(bool value) override;
        virtual void set_flip_enabled(bool value) override;
        virtual void set_hide_enabled(bool value) override;
        virtual void set_highlight(bool value) override;

        /// Set the size of the host window.
        void set_host_size(const Size& size) override;
        virtual void set_level(const std::string& name, trlevel::LevelVersion version) override;
        virtual void set_max_rooms(uint32_t rooms) override;
        virtual void set_measure_distance(float distance) override;
        virtual void set_measure_position(const Point& position) override;
        virtual void set_minimap_highlight(uint16_t x, uint16_t z) override;
        virtual void set_pick(const PickInfo& info, const PickResult& pick_result) override;
        virtual void set_remove_waypoint_enabled(bool value) override;
        virtual void set_selected_room(const std::shared_ptr<IRoom>& room) override;
        virtual void set_settings(const UserSettings& settings) override;
        virtual void set_show_context_menu(bool value) override;
        virtual void set_show_hidden_geometry(bool value) override;
        virtual void set_show_measure(bool value) override;
        virtual void set_show_minimap(bool value) override;
        virtual void set_show_tooltip(bool value) override;
        virtual void set_show_triggers(bool value) override;
        virtual void set_show_water(bool value) override;
        virtual void set_show_wireframe(bool value) override;
        virtual void set_show_bounding_boxes(bool value) override;
        virtual void set_use_alternate_groups(bool value) override;
        virtual void set_visible(bool value) override;
        virtual bool show_hidden_geometry() const override;
        virtual bool show_triggers() const override;
        virtual bool show_water() const override;
        virtual bool show_wireframe() const override;
        virtual bool show_bounding_boxes() const override;
        virtual bool show_context_menu() const override;
        virtual void toggle_settings_visibility() override;
        virtual void print_console(const std::wstring& text) override;
        virtual void initialise_input() override;
    private:
        void generate_tool_window(const IViewOptions::Source& view_options_source, const ITextureStorage& texture_storage);
        void initialise_camera_controls(ui::Control& parent);
        void register_change_detection(ui::Control* control);

        TokenStore _token_store;
        input::Mouse _mouse;
        Window _window;
        UserSettings _settings;
        ui::IInput::Source _input_source;
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::IRenderer> _ui_renderer;
        std::unique_ptr<ui::IInput> _ui_input;
        std::unique_ptr<ContextMenu> _context_menu;
        std::unique_ptr<GoTo> _go_to;
        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<IViewOptions> _view_options;
        std::unique_ptr<Toolbar> _toolbar;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<ISettingsWindow> _settings_window;
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
