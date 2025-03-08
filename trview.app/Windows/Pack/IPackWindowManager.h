#pragma once

#include "IPackWindow.h"

namespace trview
{
    struct IPackWindowManager
    {
        virtual ~IPackWindowManager() = 0;
        virtual std::weak_ptr<IPackWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_pack(const std::weak_ptr<trlevel::IPack>& pack) = 0;

        Event<std::string> on_level_open;
    };
}
