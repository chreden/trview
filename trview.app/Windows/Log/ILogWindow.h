#pragma once

namespace trview
{
    struct ILogWindow
    {
        virtual ~ILogWindow() = 0;
        virtual void render() = 0;
    };
}