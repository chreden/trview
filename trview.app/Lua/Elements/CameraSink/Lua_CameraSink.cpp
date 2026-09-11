module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaCameraSink;

import :Lua;
import :LuaRoom;
import :LuaVector3;
import :LuaTrigger;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int camera_sink_metatable = LUA_NOREF;

            int camera_sink_index(lua_State* L)
            {
                auto camera_sink = lua::get_userdata<std::shared_ptr<ICameraSink>>(L, 1);
                const std::string key = lua_tostring(L, 2);
                if (key == "box_index")
                {
                    lua_pushinteger(L, camera_sink->box_index());
                    return 1;
                }
                else if (key == "flag")
                {
                    lua_pushinteger(L, camera_sink->flag());
                    return 1;
                }
                else if (key == "inferred_rooms")
                {
                    return push_list_p(L, camera_sink->inferred_rooms(), create_room);
                }
                else if (key == "number")
                {
                    lua_pushinteger(L, camera_sink->number());
                    return 1;
                }
                else if (key == "persistent")
                {
                    lua_pushboolean(L, camera_sink->persistent());
                    return 1;
                }
                else if (key == "position")
                {
                    create_vector3(L, camera_sink->position() * trlevel::Scale);
                    return 1;
                }
                else if (key == "room")
                {
                    return create_room(L, camera_sink->room().lock());
                }
                else if (key == "strength")
                {
                    lua_pushinteger(L, camera_sink->strength());
                    return 1;
                }
                else if (key == "type")
                {
                    lua_pushstring(L, to_string(camera_sink->type()).c_str());
                    return 1;
                }
                else if (equals_any(key, "triggered_by", "trigger_references"))
                {
                    return push_list_p(L, camera_sink->triggers(), create_trigger);
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, camera_sink->visible());
                    return 1;
                }
                return 0;
            }

            int camera_sink_newindex(lua_State* L)
            {
                auto camera_sink = lua::get_userdata<std::shared_ptr<ICameraSink>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "type")
                {
                    if (auto level = camera_sink->level().lock())
                    {
                        const char* type_str = lua_tostring(L, -1);
                        if (type_str)
                        {
                            std::string type = type_str;
                            if (type == "Camera")
                            {
                                camera_sink->set_type(ICameraSink::Type::Camera);
                            }
                            else if (type == "Sink")
                            {
                                camera_sink->set_type(ICameraSink::Type::Sink);
                            }
                            else
                            {
                                return luaL_error(L, "%s is not a valid type for Camera/Sink. Valid values are \"Camera\" and \"Sink\"", type.c_str());
                            }
                        }
                        else
                        {
                            return luaL_error(L, "nil is not a valid type for Camera/Sink. Valid values are \"Camera\" and \"Sink\"");
                        }
                    }
                }
                else if (key == "visible")
                {
                    camera_sink->set_visible(lua_toboolean(L, -1));
                }

                return 0;
            }
        }

        void camera_sink_register(lua_State* L)
        {
            camera_sink_metatable = store_metatable(L,
                {
                    { "__index", camera_sink_index },
                    { "__newindex", camera_sink_newindex },
                    { "__gc", default_gc<std::shared_ptr<ICameraSink>> }
                });
        }

        int create_camera_sink(lua_State* L, std::shared_ptr<ICameraSink> camera_sink)
        {
            create_userdata(L, camera_sink);
            assign_metatable(L, camera_sink_metatable);
            return 1;
        }
    }
}

