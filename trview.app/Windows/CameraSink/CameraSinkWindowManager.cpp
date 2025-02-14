#include "CameraSinkWindowManager.h"
#include "../../Resources/resource.h"
#include "../../Elements/CameraSink/ICameraSink.h"
#include "../../Elements/IRoom.h"

namespace trview
{
    namespace
    {
        auto find_level(auto&& levels, auto&& level)
        {
            return std::ranges::find_if(levels, [=](auto&& l) { return l.level.lock() == level.lock(); });
        }
    }

    ICameraSinkWindowManager::~ICameraSinkWindowManager()
    {
    }

    CameraSinkWindowManager::CameraSinkWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ICameraSinkWindow::Source& camera_sink_window_source)
        : _camera_sink_window_source(camera_sink_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'K') += [&]() { create_window(); };
    }

    void CameraSinkWindowManager::add_level(const std::weak_ptr<ILevel>& level)
    {
        _levels.push_back({ .level = level });
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
    }

    std::weak_ptr<ICameraSinkWindow> CameraSinkWindowManager::create_window()
    {
        auto window = _camera_sink_window_source();
        window->on_camera_sink_selected += on_camera_sink_selected;
        window->on_trigger_selected += on_trigger_selected;
        window->on_scene_changed += on_scene_changed;
        for (auto& level : _levels)
        {
            window->add_level(level.level);
            window->set_current_room(level.room);
            window->set_selected_camera_sink(level.camera_sink);
        }
        
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

    void CameraSinkWindowManager::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        if (const auto camera_sink_ptr = camera_sink.lock())
        {
            const auto found = find_level(_levels, camera_sink_ptr->level());
            if (found != _levels.end())
            {
                found->camera_sink = camera_sink;
                for (auto& window : _windows)
                {
                    window.second->set_selected_camera_sink(camera_sink);
                }
            }
        }
    }

    void CameraSinkWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        if (const auto room_ptr = room.lock())
        {
            const auto found = find_level(_levels, room_ptr->level());
            if (found != _levels.end())
            {
                found->room = room;
                for (auto& window : _windows)
                {
                    window.second->set_current_room(room);
                }
            }
        }
    }
}
