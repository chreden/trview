/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/ITrigger.h>
#include "ITriggersWindow.h"
#include "CollapsiblePanel.h"
#include <trview.common/Windows/IClipboard.h>

namespace trview
{
    namespace ui
    {
        class Checkbox;
        class Dropdown;
    }

    class TriggersWindow final : public ITriggersWindow, public CollapsiblePanel
    {
    public:
        struct Names
        {
            static const std::string add_to_route_button;
            static const std::string filter_dropdown;
            static const std::string stats_listbox;
            static const std::string sync_trigger_checkbox;
            static const std::string track_room_checkbox;
            static const std::string triggers_listbox;
            static const std::string trigger_commands_listbox;
        };

        explicit TriggersWindow(const graphics::IDeviceWindow::Source& device_window_source, const ui::render::IRenderer::Source& renderer_source,
            const ui::IInput::Source& input_source, const Window& parent, const std::shared_ptr<IClipboard>& clipboard);
        virtual ~TriggersWindow() = default;
        virtual void render(bool vsync) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void update_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void clear_selected_trigger() override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual std::weak_ptr<ITrigger> selected_trigger() const override;
    protected:
        virtual void update_layout() override;
    private:
        void populate_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        void set_track_room(bool value);
        void set_sync_trigger(bool value);
        void load_trigger_details(const ITrigger& trigger);
        void apply_filters();

        ui::StackPanel* _controls;
        ui::Checkbox* _track_room_checkbox;
        ui::Listbox*  _triggers_list;
        ui::Listbox*  _stats_list;
        ui::Listbox*  _command_list;
        ui::Dropdown* _command_filter;

        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;

        /// Whether the trigger window is tracking the current room.
        bool _track_room{ false };
        /// The current room number selected for tracking.
        uint32_t _current_room{ 0u };
        /// Whether the room tracking filter has been applied.
        bool _filter_applied{ false };
        bool _sync_trigger{ true };
        std::weak_ptr<ITrigger> _selected_trigger;
        std::vector<TriggerCommandType> _selected_commands;
        std::shared_ptr<IClipboard> _clipboard;
    };
}
