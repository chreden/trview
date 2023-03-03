#include "Plugins.h"

namespace trview
{
    IPlugins::~IPlugins()
    {
    }

    Plugins::Plugins(const std::shared_ptr<IFiles>& files,
        const IPlugin::Source& plugin_source,
        const UserSettings& settings)
    {
        for (const auto& directory : settings.plugin_directories)
        {
            const auto plugins = files->get_directories(directory);
            for (const auto& plugin : plugins)
            {
                _plugins.push_back(plugin_source(plugin.path));
            }
        }
    }

    void Plugins::initialise(IApplication* application)
    {
        for (auto& plugin : _plugins)
        {
            plugin->initialise(application);
        }
    }
}
