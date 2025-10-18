/// @file RoomsWindowManager.h
/// @brief Controls and creates RoomsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/IShortcuts.h>

#include "../Settings/UserSettings.h"
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
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        std::weak_ptr<ITrigger> selected_trigger() const;
        void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        void set_level_version(trlevel::LevelVersion version) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void set_rooms(const std::vector<std::weak_ptr<IRoom>>& items) override;
        std::weak_ptr<IRoomsWindow> create_window() override;
        void update(float delta) override;
        void set_floordata(const std::vector<uint16_t>& data) override;
        void set_ng_plus(bool value) override;
        void set_trng(bool value) override;
        std::vector<std::weak_ptr<IRoomsWindow>> windows() const override;
    private:
        std::vector<std::weak_ptr<IItem>> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<ITrigger> _selected_trigger;
        std::weak_ptr<IItem> _selected_item;
        IRoomsWindow::Source _rooms_window_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        UserSettings _settings;
        std::vector<uint16_t> _floordata;
        std::vector<std::weak_ptr<ICameraSink>> _all_camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        std::weak_ptr<ILight> _selected_light;
        std::weak_ptr<ISector> _selected_sector;
        bool _ng_plus{ false };
        bool _trng{ false };
    };
}

