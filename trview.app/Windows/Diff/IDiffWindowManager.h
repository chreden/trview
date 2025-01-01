#pragma once

#include <memory>

namespace trview
{
    struct IDiffWindow;
    struct ILevel;

    struct IDiffWindowManager
    {
        virtual ~IDiffWindowManager() = 0;
        virtual std::weak_ptr<IDiffWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
    };
}
