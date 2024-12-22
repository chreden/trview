/// @file RoomsWindowManager.h
/// @brief Controls and creates RoomsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/IShortcuts.h>
#include "../Elements/IItem.h"
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
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_map_colours(const MapColours& colours) override;
        virtual void set_room(const std::weak_ptr<IRoom>& room) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& items) override;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual std::weak_ptr<IRoomsWindow> create_window() override;
        virtual void update(float delta) override;
        virtual void set_floordata(const std::vector<uint16_t>& data) override;
        virtual void set_selected_light(const std::weak_ptr<ILight>& light) override;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        void set_ng_plus(bool value) override;
        void set_trng(bool value) override;
    private:
        std::vector<std::weak_ptr<IItem>> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<ITrigger> _selected_trigger;
        std::weak_ptr<IItem> _selected_item;
        IRoomsWindow::Source _rooms_window_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        MapColours _map_colours;
        std::vector<uint16_t> _floordata;
        std::vector<std::weak_ptr<ICameraSink>> _all_camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        std::weak_ptr<ILight> _selected_light;
        bool _ng_plus{ false };
        bool _trng{ false };
    };
}

