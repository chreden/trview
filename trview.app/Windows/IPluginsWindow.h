#pragma once

namespace trview
{
    struct IPluginsWindow
    {
        virtual ~IPluginsWindow() = 0;
        virtual void render() = 0;
    };
}
