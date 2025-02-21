#include "PluginsWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IPluginsWindowManager::~IPluginsWindowManager()
    {
    }

    PluginsWindowManager::PluginsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IPluginsWindow::Source& plugins_window_source)
        : MessageHandler(window), _source(plugins_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'P') += [&]() { create_window(); };
    }

    std::optional<int> PluginsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_PLUGINS)
        {
            create_window();
        }
        return {};
    }

    void PluginsWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<IPluginsWindow> PluginsWindowManager::create_window()
    {
        auto plugins_window = _source();
        plugins_window->on_settings += on_settings;
        plugins_window->set_settings(_settings);
        return add_window(plugins_window);
    }

    void PluginsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }

    void PluginsWindowManager::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        for (auto& window : _windows)
        {
            window.second->set_settings(settings);
        }
    }
}

