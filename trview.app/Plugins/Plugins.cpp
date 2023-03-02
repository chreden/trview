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
            const auto plugins = files->get_files(directory, "\\*.zip");
            for (const auto& plugin : plugins)
            {
                const auto bytes = files->load_file(plugin.path);
                if (bytes)
                {
                    _plugins.push_back(plugin_source(bytes.value()));
                }
            }
        }
    }
}
