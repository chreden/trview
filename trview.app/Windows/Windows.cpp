#include "Windows.h"

namespace trview
{
    IWindows::~IWindows()
    {
    }

    Windows::~Windows()
    {
    }

    Windows::Windows(
        std::unique_ptr<IItemsWindowManager> items_windows,
        std::unique_ptr<ILightsWindowManager> lights_windows,
        std::unique_ptr<IRoomsWindowManager> rooms_windows,
        std::unique_ptr<ITriggersWindowManager> triggers_windows)
        : _items_windows(std::move(items_windows)), _lights_windows(std::move(lights_windows)),
        _rooms_windows(std::move(rooms_windows)), _triggers_windows(std::move(triggers_windows))
    {
        _items_windows->on_item_selected += on_item_selected;
        _items_windows->on_item_visibility += on_item_visibility;
        _items_windows->on_trigger_selected += on_trigger_selected;
        // TODO: Add to route:
        // _token_store += _items_windows->on_add_to_route += [this](const auto& item)
        // {
        //     add_waypoint(item.position(), Vector3::Down, item.room(), IWaypoint::Type::Entity, item.number());
        // };

        _lights_windows->on_light_selected += on_light_selected;
        _lights_windows->on_light_visibility += on_light_visibility;

        _rooms_windows->on_item_selected += on_item_selected;
        _rooms_windows->on_trigger_selected += on_trigger_selected;
        _rooms_windows->on_light_selected += on_light_selected;
        _rooms_windows->on_room_selected += on_room_selected;
        _rooms_windows->on_room_visibility += on_room_visibility;
        _rooms_windows->on_camera_sink_selected += on_camera_sink_selected;
        _rooms_windows->on_sector_hover += on_sector_selected;

        _token_store += _triggers_windows->on_camera_sink_selected += [this](const auto& index)
        {
            if (auto level = _level.lock())
            {
                if (auto sink = level->camera_sink(index).lock())
                {
                    on_camera_sink_selected(sink);
                }
            }
        };
        _triggers_windows->on_item_selected += on_item_selected;
        _triggers_windows->on_trigger_selected += on_trigger_selected;
        // TODO: Add to route
    }

    void Windows::render()
    {
        _items_windows->render();
        _lights_windows->render();
        _rooms_windows->render();
        _triggers_windows->render();
    }

    void Windows::set_item_visibility(const Item& item, bool value)
    {
        _items_windows->set_item_visible(item, value);
    }

    void Windows::set_level(std::shared_ptr<ILevel> level)
    {
        _level = level;

        _items_windows->set_items(level->items());
        _items_windows->set_triggers(level->triggers());
        _items_windows->set_level_version(level->version());
        _items_windows->set_model_checker([&](uint32_t id) { return level->has_model(id); });

        _lights_windows->set_level_version(level->version());
        _lights_windows->set_lights(level->lights());

        _rooms_windows->set_level_version(level->version());
        _rooms_windows->set_items(level->items());
        _rooms_windows->set_floordata(level->floor_data());
        _rooms_windows->set_rooms(level->rooms());

        _triggers_windows->set_items(level->items());
        _triggers_windows->set_triggers(level->triggers());
    }

    void Windows::set_room(uint32_t room)
    {
        _items_windows->set_room(room);
        _lights_windows->set_room(room);
        _rooms_windows->set_room(room);
        _triggers_windows->set_room(room);
    }

    void Windows::set_selected_camera_sink(std::weak_ptr<ICameraSink> camera_sink)
    {
        _rooms_windows->set_selected_camera_sink(camera_sink);
    }

    void Windows::set_selected_item(const Item& item)
    {
        _items_windows->set_selected_item(item);
        _rooms_windows->set_selected_item(item);
    }

    void Windows::set_selected_light(std::weak_ptr<ILight> light)
    {
        _lights_windows->set_selected_light(light);
        _rooms_windows->set_selected_light(light);
    }

    void Windows::set_selected_trigger(std::weak_ptr<ITrigger> trigger)
    {
        _rooms_windows->set_selected_trigger(trigger);
        _triggers_windows->set_selected_trigger(trigger);
    }

    void Windows::set_settings(const UserSettings& settings)
    {
        _rooms_windows->set_map_colours(settings.map_colours);
    }

    void Windows::startup(const UserSettings& settings)
    {
        _rooms_windows->set_map_colours(settings.map_colours);

        if (settings.items_startup)
        {
            _items_windows->create_window();
        }

        if (settings.rooms_startup)
        {
            _rooms_windows->create_window();
        }

        if (settings.triggers_startup)
        {
            _triggers_windows->create_window();
        }
    }

    void Windows::update(float elapsed)
    {
        _items_windows->update(elapsed);
        _lights_windows->update(elapsed);
        _rooms_windows->update(elapsed);
        _triggers_windows->update(elapsed);
    }
}
