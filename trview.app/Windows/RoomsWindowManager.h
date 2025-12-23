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
        std::weak_ptr<IRoomsWindow> create_window() override;
        void update(float delta) override;
        std::vector<std::weak_ptr<IRoomsWindow>> windows() const override;
    private:
        IRoomsWindow::Source _rooms_window_source;
    };
}

