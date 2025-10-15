#include "Windows.h"
#include "Elements/ILevel.h"
#include "Elements/IRoom.h"
#include "Settings/UserSettings.h"

#include "About/AboutWindowManager.h"
#include "CameraSink/ICameraSinkWindowManager.h"
#include "Console/IConsoleManager.h"
#include "Diff/IDiffWindowManager.h"
#include "IItemsWindowManager.h"
#include "ILightsWindowManager.h"
#include "Log/ILogWindowManager.h"
#include "Plugins/IPluginsWindowManager.h"
#include "IRoomsWindowManager.h"
#include "IRouteWindowManager.h"
#include "Sounds/ISoundsWindowManager.h"
#include "Statics/IStaticsWindowManager.h"
#include "Textures/ITexturesWindowManager.h"
#include "ITriggersWindowManager.h"
#include "Pack/IPackWindowManager.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    IWindows::~IWindows()
    {
    }

    Windows::Windows(
        std::unique_ptr<IAboutWindowManager> about_window_manager,
        std::unique_ptr<ICameraSinkWindowManager> camera_sink_windows,
        std::unique_ptr<IConsoleManager> console_manager,
        std::unique_ptr<IDiffWindowManager> diff_window_manager,
        std::shared_ptr<IItemsWindowManager> items_window_manager,
        std::unique_ptr<ILightsWindowManager> lights_window_manager,
        std::unique_ptr<ILogWindowManager> log_window_manager,
        std::unique_ptr<IPackWindowManager> pack_window_manager,
        std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
        std::shared_ptr<IRoomsWindowManager> rooms_window_manager,
        std::unique_ptr<IRouteWindowManager> route_window_manager,
        std::unique_ptr<ISoundsWindowManager> sounds_window_manager,
        std::unique_ptr<IStaticsWindowManager> statics_window_manager,
        std::unique_ptr<ITexturesWindowManager> textures_window_manager,
        std::unique_ptr<ITriggersWindowManager> triggers_window_manager)
        : _about_windows(std::move(about_window_manager)), _camera_sink_windows(std::move(camera_sink_windows)), _console_manager(std::move(console_manager)),
        _diff_windows(std::move(diff_window_manager)), _items_windows(items_window_manager), _lights_windows(std::move(lights_window_manager)),
        _log_windows(std::move(log_window_manager)), _plugins_windows(std::move(plugins_window_manager)), _rooms_windows(rooms_window_manager),
        _route_window(std::move(route_window_manager)), _sounds_windows(std::move(sounds_window_manager)), _statics_windows(std::move(statics_window_manager)),
        _textures_windows(std::move(textures_window_manager)), _triggers_windows(std::move(triggers_window_manager)), _pack_windows(std::move(pack_window_manager))
    {
        _camera_sink_windows->on_camera_sink_selected += on_camera_sink_selected;
        _camera_sink_windows->on_flyby_node_selected += on_flyby_node_selected;
        _camera_sink_windows->on_trigger_selected += on_trigger_selected;

        _diff_windows->on_item_selected += on_item_selected;
        _diff_windows->on_light_selected += on_light_selected;
        _diff_windows->on_trigger_selected += on_trigger_selected;
        _diff_windows->on_diff_ended += on_diff_ended;
        _diff_windows->on_camera_sink_selected += on_camera_sink_selected;
        _diff_windows->on_static_mesh_selected += on_static_selected;
        _diff_windows->on_sound_source_selected += on_sound_source_selected;
        _diff_windows->on_room_selected += on_room_selected;
        _token_store += _diff_windows->on_sector_selected += [this](auto sector)
            {
                on_sector_selected(sector);
                _rooms_windows->set_selected_sector(sector);
            };

        _token_store += _items_windows->on_add_to_route += [this](auto item)
            {
                if (auto item_ptr = item.lock())
                {
                    add_waypoint(item_ptr->position(), Vector3::Down, item_room(item_ptr), IWaypoint::Type::Entity, item_ptr->number());
                }
            };
        _items_windows->on_item_selected += on_item_selected;
        _items_windows->on_trigger_selected += on_trigger_selected;

        _lights_windows->on_light_selected += on_light_selected;
            
        _pack_windows->on_level_open += on_level_open;

        _plugins_windows->on_settings += on_settings;

        _rooms_windows->on_camera_sink_selected += on_camera_sink_selected;
        _rooms_windows->on_item_selected += on_item_selected;
        _rooms_windows->on_light_selected += on_light_selected;
        _rooms_windows->on_room_selected += on_room_selected;
        _rooms_windows->on_sector_hover += on_sector_hover;
        _rooms_windows->on_static_mesh_selected += on_static_selected;
        _rooms_windows->on_trigger_selected += on_trigger_selected;

        _route_window->on_waypoint_selected += on_waypoint_selected;
        _route_window->on_item_selected += on_item_selected;
        _route_window->on_trigger_selected += on_trigger_selected;
        _route_window->on_route_open += on_route_open;
        _route_window->on_route_reload += on_route_reload;
        _route_window->on_route_save += on_route_save;
        _route_window->on_route_save_as += on_route_save_as;
        _route_window->on_window_created += on_route_window_created;
        _route_window->on_level_switch += on_level_switch;
        _route_window->on_new_route += on_new_route;
        _route_window->on_new_randomizer_route += on_new_randomizer_route;

        _sounds_windows->on_sound_source_selected += on_sound_source_selected;

        _statics_windows->on_static_selected += on_static_selected;

        _triggers_windows->on_item_selected += on_item_selected;
        _triggers_windows->on_trigger_selected += on_trigger_selected;
        _token_store += _triggers_windows->on_add_to_route += [this](auto trigger)
            {
                if (auto trigger_ptr = trigger.lock())
                {
                    add_waypoint(trigger_ptr->position(), Vector3::Down, trigger_room(trigger_ptr), IWaypoint::Type::Trigger, trigger_ptr->number());
                }
            };
        _triggers_windows->on_camera_sink_selected += on_camera_sink_selected;
        _triggers_windows->on_flyby_node_selected += on_flyby_node_selected;
    }

    bool Windows::is_route_window_open() const
    {
        return _route_window->is_window_open();
    }

    void Windows::update(float elapsed)
    {
        _camera_sink_windows->update(elapsed);
        _items_windows->update(elapsed);
        _lights_windows->update(elapsed);
        _plugins_windows->update(elapsed);
        _rooms_windows->update(elapsed);
        _route_window->update(elapsed);
        _statics_windows->update(elapsed);
        _triggers_windows->update(elapsed);
    }

    void Windows::render()
    {
        _about_windows->render();
        _camera_sink_windows->render();
        _console_manager->render();
        _diff_windows->render();
        _items_windows->render();
        _lights_windows->render();
        _log_windows->render();
        _pack_windows->render();
        _plugins_windows->render();
        _rooms_windows->render();
        _route_window->render();
        _sounds_windows->render();
        _statics_windows->render();
        _textures_windows->render();
        _triggers_windows->render();
    }

    void Windows::select(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _camera_sink_windows->set_selected_camera_sink(camera_sink);
        _rooms_windows->set_selected_camera_sink(camera_sink);
    }

    void Windows::select(const std::weak_ptr<IFlybyNode>& flyby_node)
    {
        _camera_sink_windows->set_selected_flyby_node(flyby_node);
    }

    void Windows::select(const std::weak_ptr<IItem>& item)
    {
        _items_windows->set_selected_item(item);
        _rooms_windows->set_selected_item(item);
    }

    void Windows::select(const std::weak_ptr<ILight>& light)
    {
        _lights_windows->set_selected_light(light);
        _rooms_windows->set_selected_light(light);
    }

    void Windows::select(const std::weak_ptr<ISoundSource>& sound_source)
    {
        _sounds_windows->select_sound_source(sound_source);
    }

    void Windows::select(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        _statics_windows->select_static(static_mesh);
    }

    void Windows::select(const std::weak_ptr<ITrigger>& trigger)
    {
        _rooms_windows->set_selected_trigger(trigger);
        _triggers_windows->set_selected_trigger(trigger);
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

        _camera_sink_windows->set_camera_sinks(new_level->camera_sinks());
        _camera_sink_windows->set_flybys(new_level->flybys());
        _camera_sink_windows->set_platform_and_version(new_level->platform_and_version());
        _diff_windows->set_level(new_level);
        _items_windows->set_items(new_level->items());
        _items_windows->set_triggers(new_level->triggers());
        _items_windows->set_level_version(new_level->version());
        _items_windows->set_model_checker([=](uint32_t id) { return new_level->has_model(id); });
        _items_windows->set_ng_plus(new_level->ng_plus());
        _lights_windows->set_level_version(new_level->version());
        _lights_windows->set_lights(new_level->lights());
        _pack_windows->set_level(new_level);
        _pack_windows->set_pack(new_level->pack());
        _rooms_windows->set_level_version(new_level->version());
        _rooms_windows->set_items(new_level->items());
        _rooms_windows->set_floordata(new_level->floor_data());
        _rooms_windows->set_rooms(new_level->rooms());
        _rooms_windows->set_ng_plus(new_level->ng_plus());
        _rooms_windows->set_trng(new_level->trng());
        _route_window->set_items(new_level->items());
        _route_window->set_triggers(new_level->triggers());
        _route_window->set_rooms(new_level->rooms());
        _sounds_windows->set_level_platform(new_level->platform());
        _sounds_windows->set_level_version(new_level->version());
        _sounds_windows->set_sound_sources(new_level->sound_sources());
        _sounds_windows->set_sound_storage(new_level->sound_storage());
        _statics_windows->set_statics(new_level->static_meshes());
        _triggers_windows->set_items(new_level->items());
        _triggers_windows->set_triggers(new_level->triggers());
        _triggers_windows->set_platform_and_version(new_level->platform_and_version());
        _textures_windows->set_texture_storage(new_level->texture_storage());

        _level_token_store.clear();
        _level_token_store += new_level->on_ng_plus += [this, level](bool value)
            {
                _items_windows->set_ng_plus(value);
                _rooms_windows->set_ng_plus(value);
            };
    }

    void Windows::set_room(const std::weak_ptr<IRoom>& room)
    {
        _camera_sink_windows->set_room(room);
        _items_windows->set_room(room);
        _lights_windows->set_room(room);
        _rooms_windows->set_room(room);
        _statics_windows->set_room(room);
        _triggers_windows->set_room(room);
    }

    void Windows::set_route(const std::weak_ptr<IRoute>& route)
    {
        _route = route;
        _route_window->set_route(route);
    }

    void Windows::set_settings(const UserSettings& settings)
    {
        _plugins_windows->set_settings(settings);
        _route_window->set_randomizer_enabled(settings.randomizer_tools);
        _route_window->set_randomizer_settings(settings.randomizer);
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
            _statics_windows->create_window();
        }

        if (settings.triggers_startup)
        {
            _triggers_windows->create_window();
        }

        set_settings(settings);
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
