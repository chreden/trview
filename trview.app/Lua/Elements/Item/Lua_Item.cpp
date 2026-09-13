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

            DirectX::SimpleMath::Vector3 scale_vector(const DirectX::SimpleMath::Vector3& v)
            {
                return v * trlevel::Scale;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "activation_flags", prop_getter<std::shared_ptr<IItem>, &IItem::activation_flags> },
                { "ai", prop_getter<std::shared_ptr<IItem>, &IItem::is_ai> },
                { "angle", prop_getter<std::shared_ptr<IItem>, &IItem::angle> },
                { "categories", prop_getter<std::shared_ptr<IItem>, &IItem::categories> },
                { "clear_body", prop_getter<std::shared_ptr<IItem>, &IItem::clear_body_flag> },
                { "invisible", prop_getter<std::shared_ptr<IItem>, &IItem::invisible_flag> },
                { "ng", prop_getter<std::shared_ptr<IItem>, &IItem::ng_plus> },
                { "number", prop_getter<std::shared_ptr<IItem>, &IItem::number> },
                { "ocb", prop_getter<std::shared_ptr<IItem>, &IItem::ocb> },
                { "position", prop_getter_with_transform<std::shared_ptr<IItem>, &IItem::position, scale_vector> },
                { "remastered_extra", prop_getter<std::shared_ptr<IItem>, &IItem::is_remastered_extra> },
                { "room", prop_getter<std::shared_ptr<IItem>, &IItem::room> },
                { "triggered_by", prop_getter<std::shared_ptr<IItem>, &IItem::triggers> },
                { "trigger_references", prop_getter<std::shared_ptr<IItem>, &IItem::triggers> },
                { "type", prop_getter<std::shared_ptr<IItem>, &IItem::type> },
                { "type_id", prop_getter<std::shared_ptr<IItem>, &IItem::type_id> },
                { "visible", prop_getter<std::shared_ptr<IItem>, &IItem::visible> },
            };
        }

        void item_register(lua_State* L)
        {
            item_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions> },
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
