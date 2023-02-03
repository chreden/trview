#include "Lua_CameraSink.h"
#include "../../../Elements/ILevel.h"
#include "../../Lua.h"
#include "../../Vector3.h"
#include "../Room/Lua_Room.h"
#include "../Trigger/Lua_Trigger.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<ICameraSink**, std::shared_ptr<ICameraSink>> camera_sinks;

            int camera_sink_index(lua_State* L)
            {
                auto camera_sink = lua::get_self<ICameraSink>(L);
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
                    if (auto level = camera_sink->level().lock())
                    {
                        std::vector<std::weak_ptr<IRoom>> inferred_rooms;
                        std::ranges::transform(camera_sink->inferred_rooms(),
                            std::back_inserter(inferred_rooms),
                            [&level](const auto& r) { return level->room(r); });
                        return push_list(L, inferred_rooms, { create_room });
                    }
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
                    if (auto level = camera_sink->level().lock())
                    {
                        return create_room(L, level->room(camera_sink->room()).lock());
                    }
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
                else if (key == "triggered_by")
                {
                    return push_list(L, camera_sink->triggers(), { create_trigger });
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
                auto camera_sink = lua::get_self<ICameraSink>(L);

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
                                level->on_level_changed();
                            }
                            else if (type == "Sink")
                            {
                                camera_sink->set_type(ICameraSink::Type::Sink);
                                level->on_level_changed();
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
                    if (auto level = camera_sink->level().lock())
                    {
                        level->set_camera_sink_visibility(camera_sink->number(), lua_toboolean(L, -1));
                    }
                }

                return 0;
            }

            int camera_sink_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
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

