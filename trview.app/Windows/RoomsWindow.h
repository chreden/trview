#pragma once

#include <trview.common/Window.h>
#include <trview.ui/Listbox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Image.h>
#include <trview.app/UI/Tooltip.h>
#include "CollapsiblePanel.h"
#include <trview.app/Elements/Item.h>
#include <trview.ui.render/MapRenderer.h>
#include "IRoomsWindow.h"
#include <trview.app/UI/IBubble.h>
#include <trview.common/Windows/IClipboard.h>
#include <trview.ui/ILoader.h>

namespace trview
{
    class RoomsWindow final : public IRoomsWindow, public CollapsiblePanel
    {
    public:
        struct Names
        {
            static const std::string sync_room;
            static const std::string track_item;
            static const std::string track_trigger;
            static const std::string rooms_listbox;
            static const std::string triggers_listbox;
            static const std::string stats_listbox;
            static const std::string minimap;
            static const std::string neighbours_listbox;
            static const std::string items_listbox;
        };

        /// Create a rooms window as a child of the specified window.
        /// @param device The graphics device
        /// @param renderer_source The function to call to get a renderer.
        /// @param parent The parent window.
        explicit RoomsWindow(const graphics::IDeviceWindow::Source& device_window_source,
            const ui::render::IRenderer::Source& renderer_source,
            const ui::render::IMapRenderer::Source& map_renderer_source,
            const ui::IInput::Source& input_source,
            const std::shared_ptr<IClipboard>& clipboard,
            const IBubble::Source& bubble_source,
            const Window& parent,
            const std::shared_ptr<ui::ILoader>& loader);
        virtual ~RoomsWindow() = default;
        virtual void clear_selected_trigger() override;
        virtual void render(bool vsync) override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_selected_item(const Item& item) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void update(float delta) override;
    private:
        void load_room_details(const std::weak_ptr<IRoom>& room_ptr);
        void bind_controls();
        void set_sync_room(bool value);
        void set_track_item(bool value);
        void set_track_trigger(bool value);
        void render_minimap();

        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;

        ui::Listbox* _rooms_list;
        ui::Listbox* _neighbours_list;
        ui::Listbox* _items_list;
        ui::Listbox* _triggers_list;
        ui::Listbox* _stats_box;
        ui::Checkbox* _sync_room_checkbox;
        ui::Checkbox* _track_trigger_checkbox;
        ui::Checkbox* _track_item_checkbox;
        ui::Window* _controls;
        ui::Image* _minimap;
        bool _sync_room{ true };
        bool _track_item{ false };
        bool _track_trigger{ false };
        uint32_t _current_room{ 0u };
        std::optional<Item> _selected_item;
        std::weak_ptr<ITrigger> _selected_trigger;

        std::unique_ptr<ui::render::IMapRenderer> _map_renderer;
        std::unique_ptr<Tooltip> _map_tooltip;
        std::unique_ptr<IBubble> _bubble;
        std::shared_ptr<IClipboard> _clipboard;
        trlevel::LevelVersion _level_version;
    };
}
