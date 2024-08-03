#include "Windows.h"
#include "Elements/ILevel.h"
#include "Elements/IRoom.h"
#include "Settings/UserSettings.h"

#include "CameraSink/ICameraSinkWindowManager.h"
#include "Plugins/IPluginsWindowManager.h"
#include "Statics/IStaticsWindowManager.h"

namespace trview
{
    IWindows::~IWindows()
    {
    }

    Windows::Windows(
        std::unique_ptr<ICameraSinkWindowManager> camera_sink_windows,
        std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
        std::unique_ptr<IStaticsWindowManager> statics_window_manager)
        : _camera_sink_windows(std::move(camera_sink_windows)), _plugins_windows(std::move(plugins_window_manager)), _statics_windows(std::move(statics_window_manager))
    {
        _camera_sink_windows->on_camera_sink_selected += on_camera_sink_selected;
        _camera_sink_windows->on_trigger_selected += on_trigger_selected;
        _camera_sink_windows->on_camera_sink_type_changed += on_scene_changed;

        _statics_windows->on_static_selected += on_static_selected;
    }

    void Windows::update(float elapsed)
    {
        _plugins_windows->update(elapsed);
        _statics_windows->update(elapsed);
    }

    void Windows::render()
    {
        _camera_sink_windows->render();
        _plugins_windows->render();
        _statics_windows->render();
    }

    void Windows::select(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _camera_sink_windows->set_selected_camera_sink(camera_sink);
    }

    void Windows::select(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        _statics_windows->select_static(static_mesh);
    }

    void Windows::set_level(const std::weak_ptr<ILevel>& level)
    {
        if (auto new_level = level.lock())
        {
            _camera_sink_windows->set_camera_sinks(new_level->camera_sinks());
            _statics_windows->set_statics(new_level->static_meshes());
        }
        else
        {
            // TODO: Clear data?
        }
    }

    void Windows::set_room(const std::weak_ptr<IRoom>& room)
    {
        _camera_sink_windows->set_room(room);
        _statics_windows->set_room(room);
    }

    void Windows::setup(const UserSettings& settings)
    {
        if (settings.camera_sink_startup)
        {
            _camera_sink_windows->create_window();
        }

        if (settings.statics_startup)
        {
            _statics_windows->create_window();
        }
    }
}


