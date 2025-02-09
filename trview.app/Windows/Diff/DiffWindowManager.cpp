#include "DiffWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IDiffWindowManager::~IDiffWindowManager()
    {
    }

    DiffWindowManager::DiffWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IDiffWindow::Source& diff_window_source)
        : _diff_window_source(diff_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'D') += [&]() { create_window(); };
    }

    std::weak_ptr<IDiffWindow> DiffWindowManager::create_window()
    {
        const auto window = _diff_window_source();
        window->set_level(_level);
        window->set_settings(_settings);
        window->on_item_selected += on_item_selected;
        window->on_light_selected += on_light_selected;
        window->on_trigger_selected += on_trigger_selected;
        window->on_diff_ended += on_diff_ended;
        window->on_camera_sink_selected += on_camera_sink_selected;
        window->on_static_mesh_selected += on_static_mesh_selected;
        window->on_sound_source_selected += on_sound_source_selected;
        window->on_room_selected += on_room_selected;
        window->on_sector_selected += on_sector_selected;
        window->on_settings += on_settings;
        return add_window(window);
    }

    std::optional<int> DiffWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_DIFF)
        {
            create_window();
        }
        return {};
    }

    void DiffWindowManager::render()
    {
        WindowManager::render();
    }

    void DiffWindowManager::set_level(const std::weak_ptr<ILevel>& level)
    {
        _level = level;
        for (auto& window : _windows)
        {
            window.second->set_level(level);
        }
    }

    void DiffWindowManager::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        for (auto& window : _windows)
        {
            window.second->set_settings(settings);
        }
    }
}

