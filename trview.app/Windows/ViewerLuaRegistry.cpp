#include "Viewer.h" 
#include <trview.common/Strings.h>

namespace trview
{
    void Viewer::register_lua ()
    {
        _lua_registry.trview_openrecent = [this] (int index)
            {
            auto i = static_cast<std::size_t> (index);
            const auto& config = settings ();
            if ( i > 0 && i <= config.recent_files.size () )
                open ( *std::next ( config.recent_files.begin (), i - 1 ) );
            };

        _lua_registry.trview_open = [this] ( const std::string& path )
            {
            open ( path );
            };

        _lua_registry.trview_currentroom = [this] () -> int
            {
            if ( _level )
                return _level->selected_room ();
            return 0;
            };

        _lua_registry.trview_currentroom_set = [this] ( int room )
            {
            select_room ( room );
            };

        _lua_registry.trview_flip = [this] () -> bool
            {
            return _level && _level->alternate_mode ();
            };

        _lua_registry.trview_flip_set = [this] ( bool flip )
            {
            set_alternate_mode ( flip );
            };

        _lua_registry.camera_currentmode = [this] () -> std::string
            {
            switch ( _camera_mode )
                {
                case CameraMode::Axis: return "axis";
                case CameraMode::Free: return "free";
                case CameraMode::Orbit: return "orbit";
                default: return "none";
                }
            };

        _lua_registry.camera_currentmode_set = [this] ( const std::string& newmode )
            {
            if ( newmode == "axis" )        set_camera_mode ( CameraMode::Axis );
            else if ( newmode == "orbit" )  set_camera_mode ( CameraMode::Orbit );
            else if ( newmode == "free" )   set_camera_mode ( CameraMode::Free );
            };

        _lua_registry.camera_position = [this] () -> std::map<std::string, float>
            {
            const ICamera& camera = current_camera ();
            const auto vec = camera.rendering_position ();
            return std::map<std::string, float> { { "x", vec.x }, { "y", vec.y }, { "z", vec.z } };
            };

        _lua_registry.camera_position_set = [this] ( const std::map<std::string, float>& map )
            {
            auto npos = current_camera ().position ();
            auto set = [&] ( const char* key, float* val ) 
                { 
                if ( map.find ( key ) != map.end () ) 
                    *val = static_cast<float>(map.at ( key )); 
                };
            set ( "x", &npos.x );
            set ( "y", &npos.y );
            set ( "z", &npos.z );
            switch ( _camera_mode )
                {
                case CameraMode::Axis:
                    [[fallthrough]];
                case CameraMode::Free:
                    _free_camera.set_position ( npos );
                    break;
                case CameraMode::Orbit:
                    _target = npos;
                    _camera.set_target ( _target );
                    break;
                }
            };

        _lua_registry.camera_yaw = [this] () -> double
        {
            const auto& camera = current_camera ();
            return static_cast<double>(camera.rotation_yaw ());
        };

        _lua_registry.camera_yaw_set = [this] (double yaw)
            {
            switch ( _camera_mode )
                {
                case CameraMode::Axis:
                    [[fallthrough]];
                case CameraMode::Free:
                    _free_camera.set_rotation_yaw ( static_cast<float>(yaw) );
                    break;
                case CameraMode::Orbit:
                    _camera.set_rotation_yaw ( static_cast<float>(yaw) );
                    break;
                }
            };

        _lua_registry.camera_pitch = [this] () -> double
            {
            const auto& camera = current_camera ();
            return static_cast<double>(camera.rotation_pitch ());
            };

        _lua_registry.camera_pitch_set = [this] ( double pitch )
            {
            switch ( _camera_mode )
                {
                case CameraMode::Axis:
                    [[fallthrough]];
                case CameraMode::Free:
                    _free_camera.set_rotation_pitch ( static_cast<float>(pitch) );
                    break;
                case CameraMode::Orbit:
                    _camera.set_rotation_pitch ( static_cast<float>(pitch) );
                    break;
                }
            };

        _lua_registry.print = [this] ( const std::string& text )
            {
            _ui->print_console ( to_utf16 ( text ) );
            };

        _lua_registry.on_crazy = [this] () -> bool
            {
            const int result = MessageBox ( 
                window(), 
                L"Script has been running for a long time and may have gone crazy. Do you want to kill it?", 
                L"Lua Script error", 
                MB_YESNO );
            return result == IDYES;
            };


        lua_init ( &_lua_registry );
    }
}