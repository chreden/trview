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
        window->set_statics(_statics);
        window->set_current_room(_current_room);
        return add_window(window);
    }

    void StaticsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }

    void StaticsWindowManager::set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics)
    {
        _statics = statics;
        for (auto& window : _windows)
        {
            window.second->set_statics(statics);
        }
    }

    void StaticsWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(room);
        }
    }
}
