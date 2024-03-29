#include "Plugins.h"

namespace trview
{
    IPlugins::~IPlugins()
    {
    }

    Plugins::Plugins(const std::shared_ptr<IFiles>& files,
        const std::shared_ptr<IPlugin>& default_plugin,
        const IPlugin::Source& plugin_source,
        const UserSettings& settings)
    {
        _plugins.push_back(default_plugin);

        for (const auto& directory : settings.plugin_directories)
        {
            const auto plugins = files->get_directories(directory);
            for (const auto& plugin : plugins)
            {
                _plugins.push_back(plugin_source(plugin.path));
            }
        }
    }

    std::vector<std::weak_ptr<IPlugin>> Plugins::plugins() const
    {
        std::vector<std::weak_ptr<IPlugin>> plugins;
        std::transform(_plugins.begin(), _plugins.end(), std::back_inserter(plugins), [](auto&& plugin) { return plugin; });
        return plugins;
    }

    void Plugins::initialise(IApplication* application)
    {
        for (auto& plugin : _plugins)
        {
            plugin->initialise(application);
        }
    }

    void Plugins::render_ui()
    {
        std::ranges::for_each(_plugins, [](auto&& p) { p->render_ui(); });
    }
}
