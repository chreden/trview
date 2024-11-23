#pragma once

namespace trview
{
    struct IAboutWindowManager
    {
        virtual ~IAboutWindowManager() = 0;
        virtual void create_window() = 0;
        virtual void render() = 0;
    };
}
