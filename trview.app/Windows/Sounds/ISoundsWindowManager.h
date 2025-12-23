#pragma once

#include <vector>
#include <memory>

namespace trview
{
    struct ISoundsWindow;
    struct ISoundsWindowManager
    {
        virtual ~ISoundsWindowManager() = 0;
        virtual std::weak_ptr<ISoundsWindow> create_window() = 0;
        virtual void render() = 0;
    };
}
