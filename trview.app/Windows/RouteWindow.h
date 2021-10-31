
#pragma once

#include <trview.ui/Listbox.h>
#include <trview.ui/TextArea.h>
#include <trview.ui/Dropdown.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/GroupBox.h>
#include "CollapsiblePanel.h"
#include <trview.common/Event.h>
#include <trview.app/Routing/IWaypoint.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Room.h>
#include <trview.common/Windows/IClipboard.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>
#include "IRouteWindow.h"
#include <trview.app/UI/IBubble.h>

namespace trview
{
    struct IRoute;

    class RouteWindow final : public IRouteWindow, public CollapsiblePanel
    {
    public:
        struct Names
        {
            static const std::string export_button;
            static const std::string import_button;
            static const std::string clear_save;
            static const std::string notes_area;
            static const std::string select_save_button;
            static const std::string waypoint_stats;
            static const std::string requires_glitch;
            static const std::string vehicle_required;
            static const std::string is_item;
            static const std::string difficulty;
        };

        /// Create a route window as a child of the specified window.
        /// @param device The graphics device
        /// @param renderer_source The function to call to get a renderer.
        /// @param parent The parent window.
        explicit RouteWindow(const graphics::IDeviceWindow::Source& device_window_source, const ui::render::IRenderer::Source& renderer_source,
            const ui::IInput::Source& input_source, const trview::Window& parent, const std::shared_ptr<IClipboard>& clipboard,
            const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files, const IBubble::Source& bubble_source);
        virtual ~RouteWindow() = default;
        virtual void render(bool vsync) override;
        virtual void set_route(IRoute* route) override;
        virtual void select_waypoint(uint32_t index) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void focus() override;
        virtual void update(float delta) override;
        virtual void set_randomizer_enabled(bool value) override;
    private:
        void load_waypoint_details(uint32_t index);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        ui::Listbox::Item create_listbox_item(uint32_t index, const IWaypoint& waypoint);

        ui::Dropdown* _colour;
        ui::Listbox* _waypoints;
        ui::Listbox* _stats;
        ui::Window* _lower_box;
        ui::TextArea* _notes_area;
        ui::Button* _select_save;
        ui::Button* _delete_waypoint;
        ui::Button* _clear_save;
        IRoute* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        IWaypoint::Type _selected_type{ IWaypoint::Type::Position };
        uint32_t       _selected_index{ 0u };
        std::shared_ptr<IClipboard> _clipboard;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::unique_ptr<IBubble> _bubble;

        // Randomizer settings:
        ui::Window* _rando_group;
        ui::Checkbox* _requires_glitch;
        ui::Checkbox* _vehicle_required;
        ui::Checkbox* _is_item;
        ui::Dropdown* _difficulty;
        bool _randomizer_enabled{ false };
    };
}
