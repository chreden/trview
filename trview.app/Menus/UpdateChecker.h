#pragma once

#include <thread>
#include <string>
#include <trview.common/Messages/IMessageSystem.h>
#include "IUpdateChecker.h"

namespace trview
{
    /// Checks github for a later release of trview. If there is one, adds a menu
    /// item so that the user can update.
    class UpdateChecker final : public IUpdateChecker
    {
    public:
        explicit UpdateChecker(const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~UpdateChecker();
        void check_for_updates();
    private:
        std::weak_ptr<IMessageSystem> _messaging;
        std::thread _thread;
        std::string _current_version;
    };
}

