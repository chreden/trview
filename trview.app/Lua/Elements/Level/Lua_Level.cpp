module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

module trview.app:LuaLevel;

import trview.lua;

import :Lua;
import :ILevel;
import :IScriptable;
import :ICameraSink;

import :LuaItem;
import :LuaCameraSink;
import :LuaLight;
import :LuaRoom;
import :LuaTrigger;
import :LuaStaticMesh;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int level_metatable = LUA_NOREF;

            int level_addscriptable(lua_State* L)
            {
                auto level = lua::get_userdata<std::shared_ptr<ILevel>>(L, 1);
                auto scriptable = lua::get_userdata<std::shared_ptr<IScriptable>>(L, -1);
                level->add_scriptable(scriptable);
                return 0;
            }

            int level_removescriptable(lua_State* L)
            {
                auto level = lua::get_userdata<std::shared_ptr<ILevel>>(L, 1);
                auto scriptable = lua::get_userdata<std::shared_ptr<IScriptable>>(L, -1);
                level->remove_scriptable(scriptable);
                return 0;
            }

            int level_index(lua_State* L)
            {
                auto level = lua::get_userdata<std::shared_ptr<ILevel>>(L, 1);
                if (!level)
                {
                    return luaL_error(L, "self nil in level __index");
                }

                const std::string key = lua_tostring(L, 2);
                if (key == "add_scriptable")
                {
                    lua_pushcfunction(L, level_addscriptable);
                    return 1;
                }
                else if (key == "items")
                {
                    return push_list_p(L, 
                        level->items() |
                        std::views::filter([](auto&& i)
                            {
                                const auto item = i.lock();
                                return item && item->ng_plus().value_or(false) == false;
                            }) |
                        std::ranges::to<std::vector>(), create_item);
                }
                else if (key == "items_ng")
                {
                    return push_list_p(L,
                        level->items() |
                        std::views::filter([](auto&& i)
                            {
                                const auto item = i.lock();
                                return item && item->ng_plus().value_or(true) == true;
                            }) |
                        std::ranges::to<std::vector>(), create_item);
                }
                else if (key == "remove_scriptable")
                {
                    lua_pushcfunction(L, level_removescriptable);
                    return 1;
                }
                else if (key == "selected_item")
                {
                    auto item = level->selected_item();
                    if (item)
                    {
                        return create_item(L, level->item(item.value()).lock());
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "selected_trigger")
                {
                    auto trigger = level->selected_trigger();
                    if (trigger)
                    {
                        return create_trigger(L, level->trigger(trigger.value()).lock());
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "version")
                {
                    lua_pushinteger(L, static_cast<int>(level->version()));
                    return 1;
                }
                
                return 0;
            }

            int level_newindex(lua_State* L)
            {
                auto level = lua::get_userdata<std::shared_ptr<ILevel>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "alternate_mode")
                {
                    luaL_checktype(L, -1, LUA_TBOOLEAN);
                    level->set_alternate_mode(lua_toboolean(L, -1));
                }
                else if (key == "selected_item")
                {
                    if (auto item = to_item(L, -1))
                    {
                        level->set_selected_item(item);
                    }
                }
                else if (key == "selected_room")
                {
                    if (auto room = to_room(L, -1))
                    {
                        level->set_selected_room(room);
                    }
                }
                else if (key == "selected_trigger")
                {
                    if (auto trigger = to_trigger(L, -1))
                    {
                        level->set_selected_trigger(trigger->number());
                    }
                }

                return 0;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "alternate_mode", prop_getter<std::shared_ptr<ILevel>, &ILevel::alternate_mode> },
                { "cameras_and_sinks", prop_getter<std::shared_ptr<ILevel>, &ILevel::camera_sinks> },
                { "filename", prop_getter<std::shared_ptr<ILevel>, &ILevel::filename> },
                { "floordata", prop_getter<std::shared_ptr<ILevel>, &ILevel::floor_data> },
                { "lights", prop_getter<std::shared_ptr<ILevel>, &ILevel::lights> },
                { "rooms", prop_getter<std::shared_ptr<ILevel>, &ILevel::rooms> },
                { "selected_room", prop_getter<std::shared_ptr<ILevel>, &ILevel::selected_room> },
                { "static_meshes", prop_getter<std::shared_ptr<ILevel>, &ILevel::static_meshes> },
                { "triggers", prop_getter<std::shared_ptr<ILevel>, &ILevel::triggers> }
            };
        }

        void level_register(lua_State* L)
        {
            level_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions, level_index> },
                    { "__newindex", level_newindex },
                    { "__gc", default_gc<std::shared_ptr<ILevel>> }
                });
        }

        int create_level(lua_State* L, const std::shared_ptr<ILevel>& level)
        {
            return create_userdata(L, level, level_metatable);
        }

        std::shared_ptr<ILevel> to_level(lua_State* L, int index)
        {
            return lua::get_userdata<std::shared_ptr<ILevel>>(L, index);
        }
    }
}
