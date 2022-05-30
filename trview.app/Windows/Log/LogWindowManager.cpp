#include "LogWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    ILogWindowManager::~ILogWindowManager()
    {
    }

    LogWindowManager::LogWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILogWindow::Source& log_window_source)
        : _log_window_source(log_window_source), MessageHandler(window)
    {

    }

    std::weak_ptr<ILogWindow> LogWindowManager::create_window()
    {
        return add_window(_log_window_source());
    }

    void LogWindowManager::render()
    {
        WindowManager::render();
    }

    std::optional<int> LogWindowManager::process_message(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_LOG)
        {
            create_window();
        }
        return {};
    }
}

