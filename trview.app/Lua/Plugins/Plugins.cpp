#include "Plugins.h"
#include "IPlugin.h"
#include "Plugin.h"
#include "../../Resources/Resource.h"

namespace trview
{
    IPlugins::~IPlugins()
    {
    }

    Plugins::Plugins(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IPlugin::Source& plugin_source, const std::shared_ptr<IFiles>& files, const UserSettings& settings)
        : MessageHandler(window), _plugin_source(plugin_source), _files(files), _settings(settings)
    {
        _token_store += shortcuts->add_shortcut(false, VK_F10) += [this]()
        {
            reload();
        };
    }

    void Plugins::reload()
    {
        _plugins.clear();
        load();
        initialise();
    }

    std::optional<int> Plugins::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_PLUGINS_RELOAD)
        {
            reload();
        }
        return {};
    }

    void Plugins::load()
    {
        // Get the plugins directory from the settings object.
        const auto plugins_directory =
            _settings.plugins_directory.empty() ?
            (_files->appdata_directory() + "\\trview\\plugins") :
            _settings.plugins_directory;
        const auto plugins = _files->files(plugins_directory);

        // Load every plugin from that directory.
        for (const auto& plugin_name : plugins)
        {
            // Create plugin instance for each.
            const auto data = _files->load_file(plugin_name);
            if (data.has_value())
            {
                _plugins.push_back(_plugin_source(data.value()));
            }
        }
    }

    void Plugins::initialise()
    {
        for (auto& plugin : _plugins)
        {
            plugin->initialise();
        }
    }
     
    void Plugins::render_ui()
    {
        for (auto& plugin : _plugins)
        {
            plugin->render_ui();
        }
    }
}
