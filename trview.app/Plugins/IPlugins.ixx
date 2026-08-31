export module trview.app:IPlugins;

import std;

import :IPlugin;

namespace trview
{
    struct IApplication;
    export struct IPlugins
    {
        virtual ~IPlugins() = 0;
        virtual std::vector<std::weak_ptr<IPlugin>> plugins() const = 0;
        virtual void initialise(IApplication* application) = 0;
        virtual void render_ui() = 0;
        virtual void reload() = 0;
    };
}