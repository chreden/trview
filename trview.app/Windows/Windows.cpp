#include "Windows.h"
#include "Elements/ILevel.h"
#include "Elements/IRoom.h"
#include "Settings/UserSettings.h"

#include "IRoomsWindowManager.h"
#include "IRouteWindowManager.h"
#include "Pack/IPackWindowManager.h"

#include "../Resources/resource.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    IWindow::~IWindow()
    {
    }

    IWindows::~IWindows()
    {
    }

    Windows::Windows(
        const Window& window,
        const IWindow::Source& about_window_source,
        const IWindow::Source& camera_sink_window_source,
        const IWindow::Source& console_window_source,
        const IWindow::Source& diff_window_source,
        const IWindow::Source& items_window_source,
        const IWindow::Source& lights_window_source,
        const IWindow::Source& log_window_source,
        std::unique_ptr<IPackWindowManager> pack_window_manager,
        const IWindow::Source& plugins_window_source,
        std::shared_ptr<IRoomsWindowManager> rooms_window_manager,
        std::unique_ptr<IRouteWindowManager> route_window_manager,
        const IWindow::Source& sounds_window_source,
        const IWindow::Source& statics_window_source,
        const IWindow::Source& textures_window_source,
        const IWindow::Source& triggers_window_source,
        const std::shared_ptr<IShortcuts>& shortcuts)
        : MessageHandler(window),
        _about_window_source(about_window_source), _camera_sink_window_source(camera_sink_window_source), _console_window_source(console_window_source),
        _diff_window_source(diff_window_source), _items_window_source(items_window_source), _lights_window_source(lights_window_source),
        _log_window_source(log_window_source), _plugins_window_source(plugins_window_source), _rooms_windows(rooms_window_manager),
        _route_window(std::move(route_window_manager)), _sounds_window_source(sounds_window_source), _statics_window_source(statics_window_source),
        _textures_window_source(textures_window_source), _triggers_window_source(triggers_window_source), _pack_windows(std::move(pack_window_manager))
    {
        // TODO: Maybe move somewhere else:
        _token_store += shortcuts->add_shortcut(false, VK_F11) += [&]() { add_window(_console_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'D') += [&]() { add_window(_diff_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'I') += [&]() { add_window(_items_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'K') += [&]() { add_window(_camera_sink_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { add_window(_lights_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'P') += [&]() { add_window(_plugins_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'S') += [&]() { add_window(_statics_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'T') += [&]() { add_window(_triggers_window_source()); };

        _pack_windows->on_level_open += on_level_open;

        _rooms_windows->on_sector_hover += on_sector_hover;

        _route_window->on_waypoint_selected += on_waypoint_selected;
        _route_window->on_route_open += on_route_open;
        _route_window->on_route_reload += on_route_reload;
        _route_window->on_route_save += on_route_save;
        _route_window->on_route_save_as += on_route_save_as;
        _route_window->on_window_created += on_route_window_created;
        _route_window->on_level_switch += on_level_switch;
        _route_window->on_new_route += on_new_route;
        _route_window->on_new_randomizer_route += on_new_randomizer_route;
    }

    bool Windows::is_route_window_open() const
    {
        return _route_window->is_window_open();
    }

    std::optional<int> Windows::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND)
        {
            switch (LOWORD(wParam))
            {
                case IDM_ABOUT:
                {
                    add_window(_about_window_source());
                    break;
                }
                case ID_WINDOWS_CONSOLE:
                {
                    add_window(_console_window_source());
                    break;
                }
                case ID_WINDOWS_DIFF:
                {
                    add_window(_diff_window_source());
                    break;
                }
                case ID_WINDOWS_ITEMS:
                {
                    add_window(_items_window_source());
                    break;
                }
                case ID_WINDOWS_LIGHTS:
                {
                    add_window(_lights_window_source());
                    break;
                }
                case ID_WINDOWS_LOG:
                {
                    add_window(_log_window_source());
                    break;
                }
                case ID_WINDOWS_PLUGINS:
                {
                    add_window(_plugins_window_source());
                    break;
                }
                case ID_WINDOWS_SOUNDS:
                {
                    add_window(_sounds_window_source());
                    break;
                }
                case ID_WINDOWS_STATICS:
                {
                    add_window(_statics_window_source());
                    break;
                }
                case ID_WINDOWS_TEXTURES:
                {
                    add_window(_textures_window_source());
                    break;
                }
                case ID_WINDOWS_TRIGGERS:
                {
                    add_window(_triggers_window_source());
                    break;
                }
            }
        }
        return {};
    }

    void Windows::update(float elapsed)
    {
        WindowManager<IWindow>::update(elapsed);

        _rooms_windows->update(elapsed);
        _route_window->update(elapsed);
    }

    void Windows::render()
    {
        WindowManager<IWindow>::render();

        _pack_windows->render();
        _rooms_windows->render();
        _route_window->render();
    }

    void Windows::select(const std::weak_ptr<IWaypoint>& waypoint)
    {
        _route_window->select_waypoint(waypoint);
    }

    void Windows::set_level(const std::weak_ptr<ILevel>& level)
    {
        const auto new_level = level.lock();
        if (!new_level)
        {
            return;
        }

        _pack_windows->set_level(new_level);
    }

    void Windows::set_route(const std::weak_ptr<IRoute>& route)
    {
        _route = route;
        _route_window->set_route(route);
    }

    void Windows::setup(const UserSettings& settings)
    {
        if (settings.camera_sink_startup)
        {
            add_window(_camera_sink_window_source());
        }

        if (settings.items_startup)
        {
            add_window(_items_window_source());
        }

        if (settings.rooms_startup)
        {
            _rooms_windows->create_window();
        }

        if (settings.route_startup)
        {
            _route_window->create_window();
        }

        if (settings.statics_startup)
        {
            add_window(_statics_window_source());
        }

        if (settings.triggers_startup)
        {
            add_window(_triggers_window_source());
        }
    }

    void Windows::add_waypoint(const Vector3& position, const Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index)
    {
        if (auto route = _route.lock())
        {
            uint32_t new_index = route->insert(position, normal, room, type, index);
            on_waypoint_selected(route->waypoint(new_index));
        }
    }
}
