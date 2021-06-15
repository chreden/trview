#pragma once

#include <thread>
#include <string>
#include <trview.common/MessageHandler.h>
#include "IUpdateChecker.h"

namespace trview
{
    /// Checks github for a later release of trview. If there is one, adds a menu
    /// item so that the user can update.
    class UpdateChecker final : public IUpdateChecker, public MessageHandler
    {
    public:
        /// Create a new instance of the UpdateChecker class.
        /// @param window The main window.
        explicit UpdateChecker(const Window& window);

        /// Destructor for UpdateChecker.
        virtual ~UpdateChecker();

        /// Check github for updates to trview.
        void check_for_updates();

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
    private:
        std::thread _thread;
        std::string _current_version;
    };
}

