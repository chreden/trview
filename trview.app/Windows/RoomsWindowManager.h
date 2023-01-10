/// @file RoomsWindowManager.h
/// @brief Controls and creates RoomsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/IShortcuts.h>
#include "../Elements/Item.h"
#include "IRoomsWindowManager.h"
#include "WindowManager.h"

namespace trview
{
    class Room;
    class Shortcuts;

    /// Controls and creates RoomsWindows.
    class RoomsWindowManager final : public IRoomsWindowManager, public WindowManager<IRoomsWindow>, public MessageHandler
    {
    public:
        /// Create an RoomsWindowManager.
        /// @param window The parent window of the rooms window.
        /// @param shortcuts The shortcuts instance.
        /// @param rooms_window_source The function to call to get a rooms window.
        explicit RoomsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRoomsWindow::Source& rooms_window_source);
        virtual ~RoomsWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        std::weak_ptr<ITrigger> selected_trigger() const;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_map_colours(const MapColours& colours) override;
        virtual void set_room(uint32_t room) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& items) override;
        virtual void set_selected_item(const Item& item) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual std::weak_ptr<IRoomsWindow> create_window() override;
        virtual void update(float delta) override;
        virtual void set_floordata(const std::vector<uint16_t>& data) override;
        virtual void set_selected_light(const std::weak_ptr<ILight>& light) override;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) override;
    private:
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        uint32_t _current_room{ 0u };
        std::weak_ptr<ITrigger> _selected_trigger;
        std::optional<Item> _selected_item;
        IRoomsWindow::Source _rooms_window_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        MapColours _map_colours;
        std::vector<uint16_t> _floordata;
        std::vector<std::weak_ptr<ICameraSink>> _all_camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        std::weak_ptr<ILight> _selected_light;
    };
}

