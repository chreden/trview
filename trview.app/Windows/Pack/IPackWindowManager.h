#pragma once

#include "IPackWindow.h"

namespace trview
{
    struct ILevel;

    struct IPackWindowManager
    {
        virtual ~IPackWindowManager() = 0;
        virtual std::weak_ptr<IPackWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;

        Event<std::string> on_level_open;
    };
}
