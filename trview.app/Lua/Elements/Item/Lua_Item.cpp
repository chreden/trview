module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaItem;

import trview.lua;

import :Lua;
import :LuaVector3;
import :LuaTrigger;
import :LuaRoom;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int item_metatable = LUA_NOREF;

            int item_index(lua_State* L)
            {
                auto item = lua::get_userdata<std::shared_ptr<IItem>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "activation_flags")
                {
                    lua_pushinteger(L, item->activation_flags());
                    return 1;
                }
                else if (key == "ai")
                {
                    lua_pushboolean(L, item->is_ai());
                    return 1;
                }
                else if (key == "angle")
                {
                    lua_pushinteger(L, item->angle());
                    return 1;
                }
                else if (key == "categories")
                {
                    lua::push_list(L, item->categories(), [](auto&& L, auto&& s) { lua_pushstring(L, s.c_str()); });
                    return 1;
                }
                else if (key == "clear_body")
                {
                    lua_pushboolean(L, item->clear_body_flag());
                    return 1;
                }
                else if (key == "invisible")
                {
                    lua_pushboolean(L, item->invisible_flag());
                    return 1;
                }
                else if (key == "ng")
                {
                    const auto ng = item->ng_plus();
                    if (ng.has_value())
                    {
                        lua_pushboolean(L, ng.value());
                        return 1;
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "number")
                {
                    lua_pushinteger(L, item->number());
                    return 1;
                }
                else if (key == "ocb")
                {
                    lua_pushinteger(L, item->ocb());
                    return 1;
                }
                else if (key == "position")
                {
                    return create_vector3(L, item->position() * trlevel::Scale);
                }
                else if (key == "remastered_extra")
                {
                    lua_pushboolean(L, item->is_remastered_extra());
                    return 1;
                }
                else if (key == "room")
                {
                    return create_room(L, item->room().lock());
                }
                else if (equals_any(key, "triggered_by", "trigger_references"))
                {
                    return push_list_p(L, item->triggers(), create_trigger);
                }
                else if (key == "type")
                {
                    lua_pushstring(L, item->type().c_str());
                    return 1;
                }
                else if (key == "type_id")
                {
                    lua_pushinteger(L, item->type_id());
                    return 1;
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, item->visible());
                    return 1;
                }

                return 0;
            }

            int item_newindex(lua_State* L)
            {
                auto item = lua::get_userdata<std::shared_ptr<IItem>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "categories")
                {
                    luaL_checktype(L, 3, LUA_TTABLE);

                    std::unordered_set<std::string> categories;
                    lua_pushnil(L);
                    while (lua_next(L, 3) != 0)
                    {
                        categories.insert(lua_tostring(L, -1));
                        lua_pop(L, 1);
                    }
                    item->set_categories(categories);
                }
                else if (key == "visible")
                {
                    item->set_visible(lua_toboolean(L, -1));
                    return 0;
                }

                return 0;
            }
        }

        void item_register(lua_State* L)
        {
            item_metatable = store_metatable(L,
                {
                    { "__index", item_index },
                    { "__newindex", item_newindex },
                    { "__gc", default_gc<std::shared_ptr<IItem>> }
                });
        }

        int create_item(lua_State* L, const std::shared_ptr<IItem>& item)
        {
            return create_userdata(L, item, item_metatable);
        }

        std::shared_ptr<IItem> to_item(lua_State* L, int index)
        {
            return get_userdata<std::shared_ptr<IItem>>(L, index);
        }
    }
}
