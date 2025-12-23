#include "TriggersWindowManager.h"
#include "../Resources/resource.h"

namespace trview
{
    ITriggersWindowManager::~ITriggersWindowManager()
    {
    }

    TriggersWindowManager::TriggersWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ITriggersWindow::Source& triggers_window_source)
        : _triggers_window_source(triggers_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'T') += [&]() { create_window(); };
    }

    std::optional<int> TriggersWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_TRIGGERS)
        {
            create_window();
        }
        return {};
    }

    void TriggersWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<ITriggersWindow> TriggersWindowManager::create_window()
    {
        auto triggers_window = _triggers_window_source();
        triggers_window->on_add_to_route += on_add_to_route;
        return add_window(triggers_window);
    }

    void TriggersWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
