#include "Plugins.h"
#include <trview.common/Messages/Message.h>
#include "../Messages/Messages.h"

namespace trview
{
    IPlugins::~IPlugins()
    {
    }

    Plugins::Plugins(const std::shared_ptr<IFiles>& files,
        const std::shared_ptr<IPlugin>& default_plugin,
        const IPlugin::Source& plugin_source,
        const UserSettings& settings)
        : _files(files), _settings(settings), _plugin_source(plugin_source)
    {
        _plugins.push_back(default_plugin);
        reload();
    }

    std::vector<std::weak_ptr<IPlugin>> Plugins::plugins() const
    {
        std::vector<std::weak_ptr<IPlugin>> plugins;
        std::transform(_plugins.begin(), _plugins.end(), std::back_inserter(plugins), [](auto&& plugin) { return plugin; });
        return plugins;
    }

    void Plugins::initialise(IApplication* application)
    {
        _application = application;
        for (auto& plugin : _plugins)
        {
            plugin->initialise(_application);
        }
    }

    void Plugins::render_ui()
    {
        std::ranges::for_each(_plugins, [](auto&& p) { p->render_ui(); });
    }

    void Plugins::reload()
    {
        // Remove all but the default plugin
        _plugins.resize(1);

        for (const auto& directory : _settings.plugin_directories)
        {
            const auto plugins = _files->get_directories(directory);
            for (const auto& plugin : plugins)
            {
                auto new_plugin = _plugin_source(plugin.path);
                _plugins.push_back(new_plugin);

                const auto plugin_setting = _settings.plugins.find(plugin.path);
                new_plugin->set_enabled(plugin_setting != _settings.plugins.end() ? plugin_setting->second.enabled : true);
            }
        }

        initialise(_application);
    }

    void Plugins::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
        }
    }
}
