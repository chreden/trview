#include "RoomsWindowManager.h"
#include "../Resources/resource.h"
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    IRoomsWindowManager::~IRoomsWindowManager()
    {
    }

    RoomsWindowManager::RoomsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRoomsWindow::Source& rooms_window_source)
        : MessageHandler(window), _rooms_window_source(rooms_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'M') += [&]() { create_window(); };
    }

    std::optional<int> RoomsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_ROOMS)
        {
            create_window();
        }
        return {};
    }

    void RoomsWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<IRoomsWindow> RoomsWindowManager::create_window()
    {
        auto rooms_window = _rooms_window_source();
        rooms_window->on_sector_hover += on_sector_hover;
        rooms_window->on_static_mesh_selected += on_static_mesh_selected;
        return add_window(rooms_window);
    }

    void RoomsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }

    std::vector<std::weak_ptr<IRoomsWindow>> RoomsWindowManager::windows() const
    {
        return _windows |
            std::views::transform([](auto&& w) -> std::weak_ptr<IRoomsWindow> { return w.second; }) |
            std::ranges::to<std::vector>();
    }
}