#include "CameraSinkWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    ICameraSinkWindowManager::~ICameraSinkWindowManager()
    {
    }

    CameraSinkWindowManager::CameraSinkWindowManager(const Window& window, const std::shared_ptr<IShortcuts>&, const ICameraSinkWindow::Source& camera_sink_window_source)
        : _camera_sink_window_source(camera_sink_window_source), MessageHandler(window)
    {
    }

    std::weak_ptr<ICameraSinkWindow> CameraSinkWindowManager::create_window()
    {
        auto window = _camera_sink_window_source();
        window->set_camera_sinks(_camera_sinks);
        window->on_camera_sink_selected += on_camera_sink_selected;
        window->on_camera_sink_visibility += on_camera_sink_visibility;
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
}
