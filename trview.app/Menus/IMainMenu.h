#pragma once

namespace trview
{
    struct IMainMenu
    {
        virtual ~IMainMenu() = 0;
        virtual void render() = 0;
    };
}
