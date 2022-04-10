#pragma once

namespace trview
{
    struct IPlugins
    {
        virtual ~IPlugins() = 0;
        /// <summary>
        /// Load plugins from the plugins directory.
        /// </summary>
        virtual void load() = 0;
        virtual void initialise() = 0;
        virtual void render_ui() = 0;
    };
}
