#pragma once

#include <trview.common/Event.h>
#include "IConsole.h"

namespace trview
{
    struct IConsoleManager
    {
        virtual ~IConsoleManager() = 0;
        virtual void render() = 0;
        virtual std::weak_ptr<IConsole> create_window() = 0;
    };
}