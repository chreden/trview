#include "Lua_CameraSink.h"
#include "../../../Elements/ILevel.h"
#include "../../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<ICameraSink**, std::shared_ptr<ICameraSink>> camera_sinks;

            int camera_sink_index(lua_State* L)
            {
                ICameraSink* camera_sink = *static_cast<ICameraSink**>(lua_touserdata(L, 1));
                const std::string key = lua_tostring(L, 2);
                if (key == "number")
                {
                    lua_pushinteger(L, camera_sink->number());
                    return 1;
                }
                return 0;
            }

            int camera_sink_newindex(lua_State* L)
            {
                ICameraSink* camera_sink = *static_cast<ICameraSink**>(lua_touserdata(L, 1));
                camera_sink;
                return 0;
            }

            int camera_sink_gc(lua_State* L)
            {
                ICameraSink** userdata = static_cast<ICameraSink**>(lua_touserdata(L, 1));
                camera_sinks.erase(userdata);
                return 0;
            }
        }

        void create_camera_sink(lua_State* L, std::shared_ptr<ICameraSink> camera_sink)
        {
            if (!camera_sink)
            {
                lua_pushnil(L);
                return;
            }

            ICameraSink** userdata = static_cast<ICameraSink**>(lua_newuserdata(L, sizeof(camera_sink.get())));
            *userdata = camera_sink.get();
            camera_sinks[userdata] = camera_sink;

            lua_newtable(L);
            lua_pushcfunction(L, camera_sink_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, camera_sink_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, camera_sink_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
        }
    }
}

