#include "PluginsWindowManager.h"

namespace trview
{
    PluginsWindowManager::PluginsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>&, const IPluginsWindow::Source& plugins_window_source, const std::weak_ptr<IPlugins>& plugins)
        : MessageHandler(window), _source(plugins_window_source), _plugins(plugins)
    {
    }

    std::optional<int> PluginsWindowManager::process_message(UINT, WPARAM, LPARAM)
    {
        return std::nullopt;
    }

    void PluginsWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<IPluginsWindow> PluginsWindowManager::create_window()
    {
        auto plugins_window = _source();
        return add_window(plugins_window);
    }

    void PluginsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}

