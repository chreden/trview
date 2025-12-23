#include "SoundsWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    ISoundsWindowManager::~ISoundsWindowManager()
    {
    }

    SoundsWindowManager::SoundsWindowManager(const Window& window, const ISoundsWindow::Source& sounds_window_source)
        : MessageHandler(window), _sounds_window_source(sounds_window_source)
    {
    }

    std::optional<int> SoundsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_SOUNDS)
        {
            create_window();
        }
        return {};
    }

    std::weak_ptr<ISoundsWindow> SoundsWindowManager::create_window()
    {
        auto sounds_window = _sounds_window_source();
        return add_window(sounds_window);
    }

    void SoundsWindowManager::render()
    {
        WindowManager::render();
    }
}
