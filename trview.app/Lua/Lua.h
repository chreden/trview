#pragma once

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <string>
#include <trview.common/Event.h>

#include <functional>

namespace trview
{
    struct IApplication;

    enum class LuaEvent
    {
        ON_RENDER,
    };

    struct LuaFunctionRegistry
    {
        // trview.openrecent (integer), opens a new level file from the recent files list, indexed by parameter
        std::function<void (int)> trview_openrecent;

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

    void lua_init(LuaFunctionRegistry * reg, IApplication* application);
    void lua_execute ( const std::string& command );
    void lua_fire_event ( LuaEvent type );
    lua_State* lua_get_state();

    namespace lua
    {
        template <typename T>
        int push_list(lua_State* L, const std::vector<std::weak_ptr<T>>& range, const std::function<void(lua_State*, const std::shared_ptr<T>&)>& func);

        template <typename T>
        int push_list(lua_State* L, const std::vector<T>& range, const std::function<void(lua_State*, const T&)>& func);

        template <typename T>
        T* get_self(lua_State* L);
    }
}

#include "Lua.inl"