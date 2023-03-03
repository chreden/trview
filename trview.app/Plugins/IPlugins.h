#pragma once

namespace trview
{
    struct IApplication;

    struct IPlugins
    {
        virtual ~IPlugins() = 0;
        virtual void initialise(IApplication* application) = 0;
    };
}