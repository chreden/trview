#include "StaticsWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IStaticsWindowManager::~IStaticsWindowManager()
    {
    }

    StaticsWindowManager::StaticsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IStaticsWindow::Source& statics_window_source)
        : MessageHandler(window), _source(statics_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'S') += [&]() { create_window(); };
    }

    std::optional<int> StaticsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_STATICS)
        {
            create_window();
        }
        return {};
    }

    void StaticsWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<IStaticsWindow> StaticsWindowManager::create_window()
    {
        auto window = _source();
        return add_window(window);
    }

    void StaticsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
