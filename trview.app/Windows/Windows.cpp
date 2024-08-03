#include "Windows.h"
#include "Elements/ILevel.h"
#include "Elements/IRoom.h"
#include "Settings/UserSettings.h"

#include "CameraSink/ICameraSinkWindowManager.h"
#include "IItemsWindowManager.h"
#include "ILightsWindowManager.h"
#include "Plugins/IPluginsWindowManager.h"
#include "IRoomsWindowManager.h"
#include "Statics/IStaticsWindowManager.h"
#include "ITriggersWindowManager.h"

namespace trview
{
    IWindows::~IWindows()
    {
    }

    Windows::Windows(
        std::unique_ptr<ICameraSinkWindowManager> camera_sink_windows,
        std::unique_ptr<IItemsWindowManager> items_window_manager,
        std::unique_ptr<ILightsWindowManager> lights_window_manager,
        std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
        std::unique_ptr<IRoomsWindowManager> rooms_window_manager,
        std::unique_ptr<IStaticsWindowManager> statics_window_manager,
        std::unique_ptr<ITriggersWindowManager> triggers_window_manager)
        : _camera_sink_windows(std::move(camera_sink_windows)), _items_windows(std::move(items_window_manager)), _lights_windows(std::move(lights_window_manager)),
        _plugins_windows(std::move(plugins_window_manager)), _rooms_windows(std::move(rooms_window_manager)), _statics_windows(std::move(statics_window_manager)),
        _triggers_windows(std::move(triggers_window_manager))
    {
        _camera_sink_windows->on_camera_sink_selected += on_camera_sink_selected;
        _camera_sink_windows->on_trigger_selected += on_trigger_selected;
        _camera_sink_windows->on_scene_changed += on_scene_changed;

        // _items_windows->on_add_to_route
        _items_windows->on_item_selected += on_item_selected;
        _items_windows->on_scene_changed += on_scene_changed;
        _items_windows->on_trigger_selected += on_trigger_selected;

        _lights_windows->on_light_selected += on_light_selected;
        _lights_windows->on_scene_changed += on_scene_changed;

        _rooms_windows->on_camera_sink_selected += on_camera_sink_selected;
        _rooms_windows->on_item_selected += on_item_selected;
        _rooms_windows->on_light_selected += on_light_selected;
        _rooms_windows->on_room_selected += on_room_selected;
        _rooms_windows->on_scene_changed += on_scene_changed;
        _rooms_windows->on_sector_hover += on_sector_hover;
        _rooms_windows->on_static_mesh_selected += on_static_selected;
        _rooms_windows->on_trigger_selected += on_trigger_selected;

        _statics_windows->on_static_selected += on_static_selected;

        _triggers_windows->on_item_selected += on_item_selected;
        _triggers_windows->on_trigger_selected += on_trigger_selected;
        // _triggers_windows->on_add_to_route
        _triggers_windows->on_camera_sink_selected += on_camera_sink_selected;
        _triggers_windows->on_scene_changed += on_scene_changed;
    }

    void Windows::update(float elapsed)
    {
        _items_windows->update(elapsed);
        _lights_windows->update(elapsed);
        _plugins_windows->update(elapsed);
        _rooms_windows->update(elapsed);
        _statics_windows->update(elapsed);
        _triggers_windows->update(elapsed);
    }

    void Windows::render()
    {
        _camera_sink_windows->render();
        _items_windows->render();
        _lights_windows->render();
        _plugins_windows->render();
        _rooms_windows->render();
        _statics_windows->render();
        _triggers_windows->render();
    }

    void Windows::select(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _camera_sink_windows->set_selected_camera_sink(camera_sink);
        _rooms_windows->set_selected_camera_sink(camera_sink);
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

    void Windows::select(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        _statics_windows->select_static(static_mesh);
    }

    void Windows::select(const std::weak_ptr<ITrigger>& trigger)
    {
        _rooms_windows->set_selected_trigger(trigger);
        _triggers_windows->set_selected_trigger(trigger);
    }

    void Windows::set_level(const std::weak_ptr<ILevel>& level)
    {
        if (auto new_level = level.lock())
        {
            _camera_sink_windows->set_camera_sinks(new_level->camera_sinks());
            _items_windows->set_items(new_level->items());
            _items_windows->set_triggers(new_level->triggers());
            _items_windows->set_level_version(new_level->version());
            _items_windows->set_model_checker([=](uint32_t id) { return new_level->has_model(id); });
            _lights_windows->set_level_version(new_level->version());
            _lights_windows->set_lights(new_level->lights());
            _rooms_windows->set_level_version(new_level->version());
            _rooms_windows->set_items(new_level->items());
            _rooms_windows->set_floordata(new_level->floor_data());
            _rooms_windows->set_rooms(new_level->rooms());
            _statics_windows->set_statics(new_level->static_meshes());
            _triggers_windows->set_items(new_level->items());
            _triggers_windows->set_triggers(new_level->triggers());
        }
        else
        {
            // TODO: Clear data?
        }
    }

    void Windows::set_room(const std::weak_ptr<IRoom>& room)
    {
        _camera_sink_windows->set_room(room);
        _lights_windows->set_room(room);
        _rooms_windows->set_room(room);
        _statics_windows->set_room(room);
        _triggers_windows->set_room(room);
    }

    void Windows::set_settings(const UserSettings& settings)
    {
        _rooms_windows->set_map_colours(settings.map_colours);
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
}


