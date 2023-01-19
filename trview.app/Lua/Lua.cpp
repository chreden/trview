#include "Lua.h"
#include <trview.common/Strings.h>
#include "trview/trview.h"

namespace trview
{

    static const int LUA_MAX_INS = 2048;
    static const int DEFAULT_MAX_TRIES = 1000;

    static int num_tries = DEFAULT_MAX_TRIES;
    static bool ignore = false;
    static lua_State* state;
    static LuaFunctionRegistry* op;
    static std::map <LuaEvent, int> callbacks;

    LuaFunctionRegistry lua_registry;

    // Function hit whenever the VM executes LUA_MAX_INS instructions.
    // This counter doesn't seem to reset, so we use "num_tries" variable to scope it to a single script/command.
    static void rescue ( lua_State* L, lua_Debug* )
    {
        if ( !ignore && num_tries-- == 0 )
        {
            bool kill = op->on_crazy ();
            if ( !kill ) // user has decided to ignore the crazy script.
            {
                ignore = true;
                num_tries = DEFAULT_MAX_TRIES;
                return;
            }
            luaL_error ( L, "Script killed by user" );
        }
    }

    // Override for Lua builtin print() to go to the UI console
    static int print ( lua_State* L )
    {
        std::ostringstream oss;

        int nargs = lua_gettop ( L );
        for ( int i = 1; i <= nargs; ++i )
        {
            if ( lua_isboolean ( L, i ) )
            {
                oss << ( lua_toboolean ( L, i ) ? "true" : "false" ) << " ";
            }
            else if ( lua_isnoneornil(L, i) )
            {
                oss << "nil "; 
            }
            else
            {
                const char* const s = luaL_checkstring ( L, i );
                if ( s )
                    oss << lua_tostring ( L, i ) << " ";
            }
        }

        op->print ( oss.str () );
        return 0;
    }

    static int add_callback ( lua_State* L, LuaEvent e )
    {
        if ( !lua_isfunction ( L, -1 ) )
            return luaL_error ( L, "given argument is not a function" );

        // remove existing reference if there is one 
        if ( callbacks.find ( e ) != callbacks.end () )
        {
            const int callback = callbacks [e];
            if ( callback != LUA_REFNIL )
                luaL_unref ( L, LUA_REGISTRYINDEX, callback );
        }

        int t = luaL_ref ( L, LUA_REGISTRYINDEX );
        callbacks [e] = t;
        return 0;
    }

    // Callback for whenever a new frame is rendered in the viewer
    static int register_onrender ( lua_State* L )
    {
        return add_callback ( L, LuaEvent::ON_RENDER );
    }

    // trview.open (str) - opens a new file in the viewer
    static int trview_open ( lua_State* L )
    {
        const char* filename = luaL_checkstring ( L, 1 );
        op->trview_open ( filename );
        return 0;
    }

    // trview.recent (int) - opens a recent file indexed by parameter
    static int trview_recent ( lua_State* L )
    {
        auto index = static_cast<int> (luaL_checkinteger ( L, 1 ));
        if ( index <= 0 )
            return luaL_error ( L, "provided index %d is not a positive integer", index );

        op->trview_openrecent ( index );
        return 0;
    }

    // trview.__index
    static int trview_index ( lua_State* L )
    {
        const std::string key = lua_tostring ( L, 2 );

        if ( key == "currentroom" )
        {
            int room = static_cast<long long> (op->trview_currentroom ());
            lua_pushinteger ( L, room );
        }
        else if ( key == "flip" )
        {
            bool flip = op->trview_flip ();
            lua_pushboolean ( L, flip );
        }
        else
            return 0;

        return 1;
    }

    // trview.__newindex
    static int trview_newindex ( lua_State* L )
    {
        const std::string key = lua_tostring ( L, 2 );

        if ( key == "currentroom" )
        {
            int room = static_cast<int>(luaL_checkinteger ( L, 3 ));
            op->trview_currentroom_set ( room );
        }
        else if ( key == "flip" )
        {
            bool flip = lua_toboolean ( L, 3 );
            op->trview_flip_set ( flip );
        }

        return 0;
    }

    // camera.__index
    static int camera_index ( lua_State* L )
    {
        std::string key = lua_tostring ( L, 2 );

        if ( key == "mode" )
        {
            std::string mode = op->camera_currentmode ();
            lua_pushstring ( L, mode.c_str () );
        }
        else if ( key == "yaw" )
        {
            double yaw = op->camera_yaw ();
            lua_pushnumber ( L, yaw );
        }
        else if ( key == "pitch" )
        {
            double pitch = op->camera_pitch ();
            lua_pushnumber ( L, pitch );
        }
        else if ( key == "x" || key == "y" || key == "z" )
        {
            auto pos = op->camera_position ();
            float val = pos [key] * 1024.0f;
            lua_pushnumber ( L, val );
        }
        else
            return 0;

        return 1;
    }

    // camera.__newindex
    static int camera_newindex ( lua_State* L )
    {
        std::string key = luaL_checkstring ( L, 2 );

        if ( key == "mode" )
        {
            std::string mode = luaL_checkstring ( L, 3 );
            op->camera_currentmode_set ( mode );
        }
        else if ( key == "yaw" )
        {
            float yaw = static_cast<float>(luaL_checknumber ( L, 3 ));
            op->camera_yaw_set ( yaw );
        }
        else if ( key == "pitch" )
        {
            float pitch = static_cast<float>(luaL_checknumber ( L, 3 ));
            op->camera_pitch_set ( pitch );
        }
        else if ( key == "x" || key == "y" || key == "z" )
        {
            float val = static_cast<float>(luaL_checknumber ( L, 3 ));
            auto pos = op->camera_position ();
            pos [key] = val / 1024.0f;
            op->camera_position_set ( pos );
        }
        return 0;
    }

    static const struct luaL_Reg register_lib[] =
    {
        { "onrender", register_onrender },
        { NULL, NULL },
    };

    static const struct luaL_Reg trview_lib[] =
    {
        { "open", trview_open },
        { "recent", trview_recent },
        { "__index", trview_index },
        { "__newindex", trview_newindex },
        { NULL, NULL },
    };

    static const struct luaL_Reg camera_lib[] =
    {
        { "__index", camera_index },
        { "__newindex", camera_newindex },
        { NULL, NULL },
    };

    void lua_init ( LuaFunctionRegistry* reg )
    {
        op = reg;
        lua_State* L = luaL_newstate ();

        luaL_openlibs ( L );
        // Hook function for when a script goes haywire.
        lua_sethook ( L, rescue, LUA_MASKCOUNT, LUA_MAX_INS );

        // register
        luaL_newlib ( L, register_lib );
        lua_setglobal ( L, "register" ); 

        // trview 
        luaL_newlib ( L, trview_lib );
        lua_pushvalue ( L, -1 );
        lua_setmetatable ( L, -2 );
        lua_setglobal ( L, "trview" );

        // camera 
        luaL_newlib ( L, camera_lib );
        lua_pushvalue ( L, -1 );
        lua_setmetatable ( L, -2 );
        lua_setglobal ( L, "camera" );

        // utility
        lua_register ( L, "print", print );

        lua::trview_register(L);

        state = L;
    }

    // executes some lua code in the VM.
    void lua_execute ( const std::string& command )
    {
        ignore = false;
        num_tries = DEFAULT_MAX_TRIES;

        if ( luaL_dostring (state, command.c_str () ) != LUA_OK )
        {
            std::string msg = lua_tostring ( state, -1 );
            if ( op )
                op->print ( msg );
        }
    }

    // fire some event and call associated callback function, if there is one registered
    void lua_fire_event ( LuaEvent type )
    {
        if ( callbacks.find ( type ) != callbacks.end () )
        {
            const int ref = callbacks [type];
            if ( ref == LUA_REFNIL )
                return;

            ignore = false;
            num_tries = DEFAULT_MAX_TRIES;
            lua_rawgeti ( state, LUA_REGISTRYINDEX, ref );
            lua_call ( state, 0, 0 );
        }
    }

    lua_State* lua_get_state()
    {
        return state;
    }
}