#pragma once

namespace trview
{
    struct IViewMenu
    {
        virtual ~IViewMenu() = 0;
        virtual void render() = 0;
    };
}
