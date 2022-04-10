#pragma once

#include <external/lua/src/lua.hpp>
#include <string>
#include <trview.common/Event.h>

#include <functional>

namespace trview
{
    enum class LuaEvent
    {
        ON_RENDER,
        ON_RENDER_UI
    };

    struct LuaFunctionRegistry
    {
        // trview.openrecent (integer), opens a new level file from the recent files list, indexed by parameter
        std::function<void (int)> trview_openrecent;

        // trview.open (path), opens a new level file
        std::function<void ( const std::string& )> trview_open;

        // review.room, gets the current room 
        std::function<int ()> trview_currentroom;

        // trview.room = integer, sets the current room we are looking at
        std::function<void ( int )> trview_currentroom_set;

        // trview.flip, gets the current flip status true | false
        std::function<bool ()> trview_flip;

        // trview.flip = true | false, sets whether we are "flipped"
        std::function<void ( bool )> trview_flip_set;
        
        // camera.currentmode, gets the current camera mode as either "orbit" | "free" | "axis"
        std::function<std::string ()> camera_currentmode;

        // camera.currentmode = "orbit" | "free" | "axis"
        std::function<void ( const std::string& )> camera_currentmode_set;

        // camera.x, camera.y, camera.z
        std::function<std::map<std::string, float> ()> camera_position;

        // camera.x = N, camera.y = N, camera.z = N
        std::function<void ( const std::map<std::string, float>& )> camera_position_set;

        // camera.yaw => integer
        std::function<double ()> camera_yaw;

        // camera.yaw = N
        std::function<void ( double )> camera_yaw_set;

        // camera.pitch => integer 
        std::function<double ()> camera_pitch;

        // camera.pitch = N
        std::function<void ( double )> camera_pitch_set;

        // print() override        
        std::function<void ( const std::string& )>  print;

        // called when a lua script has gone crazy and may be stuck 
        // should return T/F for whether to kill the script
        std::function<bool ()> on_crazy;
    }; 

    extern LuaFunctionRegistry lua_registry;

    void lua_init ( LuaFunctionRegistry * reg );
    void lua_execute ( const std::string& command );
    void lua_fire_event ( LuaEvent type );

    namespace lua
    {
        /// <summary>
        /// Set a boolean field on the table at the top of the stack.
        /// </summary>
        void set_boolean(lua_State* L, const std::string& name, bool value, int position = -2);

        /// <summary>
        /// Set an integer field on the table at the top of the stack.
        /// </summary>
        void set_integer(lua_State* L, const std::string& name, int value, int position = -2);

        /// <summary>
        /// Set a string field on the table at the top of the stack.
        /// </summary>
        void set_string(lua_State* L, const std::string& name, const std::string& value, int position = -2);
    }
}
