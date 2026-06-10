#pragma once

namespace trview
{
    struct ILevel;
    struct ILevelInfo
    {
        virtual ~ILevelInfo() = 0;
        virtual void render() = 0;
    };
}
