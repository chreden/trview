#pragma once

#include "ILogWindow.h"

namespace trview
{
    struct ILogWindowManager
    {
        virtual ~ILogWindowManager() = 0;
        virtual std::weak_ptr<ILogWindow> create_window() = 0;
        virtual void render() = 0;
    };
}
