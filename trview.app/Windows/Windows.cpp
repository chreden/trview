#include "Windows.h"
#include "Elements/ILevel.h"
#include "Elements/IRoom.h"
#include "Settings/UserSettings.h"

#include "About/AboutWindowManager.h"
#include "CameraSink/ICameraSinkWindowManager.h"
#include "Console/IConsoleManager.h"
#include "Diff/IDiffWindowManager.h"
#include "IItemsWindowManager.h"
#include "Log/ILogWindowManager.h"
#include "Plugins/IPluginsWindowManager.h"
#include "IRoomsWindowManager.h"
#include "IRouteWindowManager.h"
#include "ITriggersWindowManager.h"
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
        std::unique_ptr<IAboutWindowManager> about_window_manager,
        std::unique_ptr<ICameraSinkWindowManager> camera_sink_windows,
        std::unique_ptr<IConsoleManager> console_manager,
        std::unique_ptr<IDiffWindowManager> diff_window_manager,
        std::shared_ptr<IItemsWindowManager> items_window_manager,
        const IWindow::Source& lights_window_source,
        std::unique_ptr<ILogWindowManager> log_window_manager,
        std::unique_ptr<IPackWindowManager> pack_window_manager,
        std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
        std::shared_ptr<IRoomsWindowManager> rooms_window_manager,
        std::unique_ptr<IRouteWindowManager> route_window_manager,
        const IWindow::Source& sounds_window_source,
        const IWindow::Source& statics_window_source,
        const IWindow::Source& textures_window_source,
        std::unique_ptr<ITriggersWindowManager> triggers_window_manager,
        const std::shared_ptr<IShortcuts>& shortcuts)
        : MessageHandler(window),
        _about_windows(std::move(about_window_manager)), _camera_sink_windows(std::move(camera_sink_windows)), _console_manager(std::move(console_manager)),
        _diff_windows(std::move(diff_window_manager)), _items_windows(items_window_manager), _lights_window_source(lights_window_source),
        _log_windows(std::move(log_window_manager)), _plugins_windows(std::move(plugins_window_manager)), _rooms_windows(rooms_window_manager),
        _route_window(std::move(route_window_manager)), _sounds_window_source(sounds_window_source), _statics_window_source(statics_window_source),
        _textures_window_source(textures_window_source), _triggers_windows(std::move(triggers_window_manager)), _pack_windows(std::move(pack_window_manager))
    {
        // TODO: Maybe move somewhere else:
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { add_window(_lights_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'S') += [&]() { add_window(_statics_window_source()); };

        _diff_windows->on_diff_ended += on_diff_ended;

        _token_store += _items_windows->on_add_to_route += [this](auto item)
            {
                if (auto item_ptr = item.lock())
                {
                    add_waypoint(item_ptr->position(), Vector3::Down, item_room(item_ptr), IWaypoint::Type::Entity, item_ptr->number());
                }
            };
            
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

        _token_store += _triggers_windows->on_add_to_route += [this](auto trigger)
            {
                if (auto trigger_ptr = trigger.lock())
                {
                    add_waypoint(trigger_ptr->position(), Vector3::Down, trigger_room(trigger_ptr), IWaypoint::Type::Trigger, trigger_ptr->number());
                }
            };
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
                case ID_WINDOWS_LIGHTS:
                {
                    add_window(_lights_window_source());
                    break;
                }
                case ID_WINDOWS_TEXTURES:
                {
                    add_window(_textures_window_source());
                    break;
                }
            }
        }
        return {};
    }

    void Windows::update(float elapsed)
    {
        WindowManager<IWindow>::update(elapsed);

        _camera_sink_windows->update(elapsed);
        _items_windows->update(elapsed);
        _plugins_windows->update(elapsed);
        _rooms_windows->update(elapsed);
        _route_window->update(elapsed);
        _triggers_windows->update(elapsed);
    }

    void Windows::render()
    {
        WindowManager<IWindow>::render();

        _about_windows->render();
        _camera_sink_windows->render();
        _console_manager->render();
        _diff_windows->render();
        _items_windows->render();
        _log_windows->render();
        _pack_windows->render();
        _plugins_windows->render();
        _rooms_windows->render();
        _route_window->render();
        _triggers_windows->render();
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
            _camera_sink_windows->create_window();
        }

        if (settings.items_startup)
        {
            _items_windows->create_window();
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
            _triggers_windows->create_window();
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
