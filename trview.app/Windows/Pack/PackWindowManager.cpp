#include "PackWindowManager.h"
#include "../../Resources/resource.h"
#include "../../Elements/ILevel.h"

namespace trview
{
    IPackWindowManager::~IPackWindowManager()
    {
    }

    PackWindowManager::PackWindowManager(const Window& window, const IPackWindow::Source& pack_window_source)
        : MessageHandler(window), _pack_window_source(pack_window_source)
    {
    }

    std::weak_ptr<IPackWindow> PackWindowManager::create_window()
    {
        auto window = _pack_window_source();
        window->on_level_open += on_level_open;
        return add_window(window);
    }

    std::optional<int> PackWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_PACK)
        {
            create_window();
        }
        return {};
    }

    void PackWindowManager::render()
    {
        WindowManager::render();
    }

    void PackWindowManager::set_level(const std::weak_ptr<ILevel>& level)
    {
        if (auto level_ptr = level.lock())
        {
            if (level_ptr->version() == trlevel::LevelVersion::Unknown &&
                level_ptr->pack().lock() &&
                _windows.empty())
            {
                create_window();
            }
        }
    }
}
