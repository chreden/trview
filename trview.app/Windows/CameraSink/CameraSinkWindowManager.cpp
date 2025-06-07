#include "CameraSinkWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    ICameraSinkWindowManager::~ICameraSinkWindowManager()
    {
    }

    CameraSinkWindowManager::CameraSinkWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ICameraSinkWindow::Source& camera_sink_window_source)
        : _camera_sink_window_source(camera_sink_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'K') += [&]() { create_window(); };
    }

    std::weak_ptr<ICameraSinkWindow> CameraSinkWindowManager::create_window()
    {
        auto window = _camera_sink_window_source();
        window->set_camera_sinks(_camera_sinks);
        window->set_flybys(_flybys);
        window->set_selected_camera_sink(_selected_camera_sink);
        window->set_current_room(_current_room);
        window->set_platform_and_version(_platform_and_version);
        window->set_settings(_settings);
        window->on_camera_sink_selected += on_camera_sink_selected;
        window->on_trigger_selected += on_trigger_selected;
        window->on_scene_changed += on_scene_changed;
        window->on_settings += on_settings;
        return add_window(window);
    }

    void CameraSinkWindowManager::render()
    {
        WindowManager::render();
    }

    std::optional<int> CameraSinkWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_CAMERA_SINK)
        {
            create_window();
        }
        return {};
    }

    void CameraSinkWindowManager::set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks)
    {
        _camera_sinks = camera_sinks;
        for (auto& window : _windows)
        {
            window.second->set_camera_sinks(_camera_sinks);
        }
    }

    void CameraSinkWindowManager::set_flybys(const std::vector<std::weak_ptr<IFlyby>>& flybys)
    {
        _flybys = flybys;
        for (auto& window : _windows)
        {
            window.second->set_flybys(_flybys);
        }
    }

    void CameraSinkWindowManager::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _selected_camera_sink = camera_sink;
        for (auto& window : _windows)
        {
            window.second->set_selected_camera_sink(_selected_camera_sink);
        }
    }

    void CameraSinkWindowManager::set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version)
    {
        _platform_and_version = platform_and_version;
        for (auto& window : _windows)
        {
            window.second->set_platform_and_version(_platform_and_version);
        }
    }

    void CameraSinkWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(_current_room);
        }
    }

    void CameraSinkWindowManager::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        for (auto& window : _windows)
        {
            window.second->set_settings(_settings);
        }
    }

    void CameraSinkWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
