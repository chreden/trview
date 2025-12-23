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
        window->set_current_room(_current_room);
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

    void CameraSinkWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(_current_room);
        }
    }

    void CameraSinkWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
