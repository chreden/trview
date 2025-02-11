#include "StaticsWindowManager.h"
#include "../../Resources/resource.h"
#include "../../Elements/IRoom.h"

namespace trview
{
    namespace
    {
        auto find_level(auto&& levels, auto&& level)
        {
            return std::ranges::find_if(levels, [=](auto&& l) { return l.level.lock() == level.lock(); });
        }
    }

    IStaticsWindowManager::~IStaticsWindowManager()
    {
    }

    StaticsWindowManager::StaticsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IStaticsWindow::Source& statics_window_source)
        : MessageHandler(window), _source(statics_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'S') += [&]() { create_window(); };
    }

    void StaticsWindowManager::add_level(const std::weak_ptr<ILevel>& level)
    {
        _levels.push_back({ .level = level });
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
    }

    std::weak_ptr<IStaticsWindow> StaticsWindowManager::create_window()
    {
        auto window = _source();
        window->on_static_selected += on_static_selected;
        for (auto& level : _levels)
        {
            window->add_level(level.level);
            window->set_current_room(level.room);
            window->set_selected_static(level.static_mesh);
        }
        return add_window(window);
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

    void StaticsWindowManager::select_static(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        if (const auto static_mesh_ptr = static_mesh.lock())
        {
            const auto found = find_level(_levels, static_mesh_ptr->level());
            if (found != _levels.end())
            {
                found->static_mesh = static_mesh;
                for (auto& window : _windows)
                {
                    window.second->set_selected_static(static_mesh);
                }
            }
        }
    }

    void StaticsWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        if (const auto room_ptr = room.lock())
        {
            const auto found = find_level(_levels, room_ptr->level());
            if (found != _levels.end())
            {
                found->room = room;
                for (auto& window : _windows)
                {
                    window.second->set_current_room(room);
                }
            }
        }
    }

    void StaticsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
