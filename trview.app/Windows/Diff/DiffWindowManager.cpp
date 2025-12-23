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
        window->on_diff_ended += on_diff_ended;
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
}

