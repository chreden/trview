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
        return add_window(_diff_window_source());
    }

    void DiffWindowManager::render()
    {
        WindowManager::render();
    }

    std::optional<int> DiffWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_DIFF)
        {
            create_window();
        }
        return {};
    }
}

