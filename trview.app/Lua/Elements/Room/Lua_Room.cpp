module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

module trview.app:LuaRoom;

import std;
import std.compat;

import trview.lua;

import :Lua;
import :LuaSector;
import :LuaItem;
import :LuaLevel;
import :LuaVector3;
import :LuaLight;
import :LuaStaticMesh;
import :LuaTrigger;
import :LuaCameraSink;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int room_metatable = LUA_NOREF;

            int get_sector(lua_State* L)
            {
                const auto room = lua::get_userdata<std::shared_ptr<IRoom>>(L, 1);
                const auto x = static_cast<int32_t>(lua_tointeger(L, 2) - 1);
                const auto z = static_cast<int32_t>(lua_tointeger(L, 3) - 1);
                return to_lua(L, room->sector(x, z).lock());
            }

            int room_hasflag(lua_State* L)
            {
                auto room = lua::get_userdata<std::shared_ptr<IRoom>>(L, 1);

                luaL_checktype(L, -1, LUA_TNUMBER);
                long long flags = lua_tointeger(L, -1);

                lua_pushboolean(L, static_cast<long long>(room->flags()) & flags);
                return 1;
            }

            int room_index(lua_State* L)
            {
                auto room = lua::get_userdata<std::shared_ptr<IRoom>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "alternate_room")
                {
                    if (auto level = room->level().lock())
                    {
                        return to_lua(L, level->room(room->alternate_room()).lock());
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "has_flag")
                {
                    lua_pushcfunction(L, room_hasflag);
                    return 1;
                }
                else if (key == "items")
                {
                    return push_list_p(L,
                        room->items() |
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
                        room->items() |
                        std::views::filter([](auto&& i)
                            {
                                const auto item = i.lock();
                                return item && item->ng_plus().value_or(true) == true;
                            }) |
                        std::ranges::to<std::vector>(), create_item);
                }
                else if (key == "position")
                {
                    const auto info = room->info();
                    return create_vector3(L, DirectX::SimpleMath::Vector3(static_cast<float>(info.x), static_cast<float>(info.yBottom), static_cast<float>(info.z)));
                }
                else if (key == "sector")
                {
                    lua_pushcfunction(L, get_sector);
                    return 1;
                }

                return 0;
            }

            int room_newindex(lua_State* L)
            {
                auto room = lua::get_userdata<std::shared_ptr<IRoom>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    room->set_visible(lua_toboolean(L, -1));
                }

                return 0;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "alternate_mode", prop_getter<std::shared_ptr<IRoom>, &IRoom::alternate_mode> },
                { "alternate_group", prop_getter<std::shared_ptr<IRoom>, &IRoom::alternate_group> },
                { "cameras_and_sinks", prop_getter<std::shared_ptr<IRoom>, &IRoom::camera_sinks> },
                { "flags", prop_getter<std::shared_ptr<IRoom>, &IRoom::flags> },
                { "level", prop_getter<std::shared_ptr<IRoom>, &IRoom::level> },
                { "lights", prop_getter<std::shared_ptr<IRoom>, &IRoom::lights> },
                { "number", prop_getter<std::shared_ptr<IRoom>, &IRoom::number> },
                { "num_x_sectors", prop_getter<std::shared_ptr<IRoom>, &IRoom::num_x_sectors> },
                { "num_z_sectors", prop_getter<std::shared_ptr<IRoom>, &IRoom::num_z_sectors> },
                { "sectors", prop_getter<std::shared_ptr<IRoom>, &IRoom::sectors> },
                { "static_meshes", prop_getter<std::shared_ptr<IRoom>, &IRoom::static_meshes> },
                { "triggers", prop_getter<std::shared_ptr<IRoom>, &IRoom::triggers> },
                { "visible", prop_getter<std::shared_ptr<IRoom>, &IRoom::visible> },
                { "water_scheme", prop_getter<std::shared_ptr<IRoom>, &IRoom::water_scheme> }
            };
        }

        void room_register(lua_State* L)
        {
            room_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions, room_index> },
                    { "__newindex", room_newindex },
                    { "__gc", default_gc<std::shared_ptr<IRoom>> }
                });

            lua_newtable(L);
            create_enum<IRoom::Flag>(L, "Flags",
                {
                    { "Water", IRoom::Flag::Water },
                    { "Bit1", IRoom::Flag::Bit1 },
                    { "Bit2", IRoom::Flag::Bit2 },
                    { "Outside", IRoom::Flag::Outside },
                    { "Bit4", IRoom::Flag::Bit4 },
                    { "Wind", IRoom::Flag::Wind },
                    { "Bit6", IRoom::Flag::Bit6 },
                    { "Bit7", IRoom::Flag::Bit7 },
                    { "Quicksand", IRoom::Flag::Quicksand },
                    { "NoLensFlare", IRoom::Flag::NoLensFlare },
                    { "Caustics", IRoom::Flag::Caustics },
                    { "WaterReflectivity", IRoom::Flag::WaterReflectivity },
                    { "Bit10", IRoom::Flag::Bit10 },
                    { "Bit11", IRoom::Flag::Bit11 },
                    { "Bit12", IRoom::Flag::Bit12 },
                    { "Bit13", IRoom::Flag::Bit13 },
                    { "Bit14", IRoom::Flag::Bit14 },
                    { "Bit15", IRoom::Flag::Bit15 }
                });
            lua_setglobal(L, "Room");
        }

        std::shared_ptr<IRoom> to_room(lua_State* L, int index)
        {
            return get_userdata<std::shared_ptr<IRoom>>(L, index);
        }

        std::shared_ptr<IRoom> to_room(lua_State* L, int index, const std::string& field_name)
        {
            lua_getfield(L, index, field_name.c_str());
            auto room = to_room(L, -1);
            lua_pop(L, 1);
            return room;
        }

        int to_lua(lua_State* L, const std::weak_ptr<IRoom>& room)
        {
            return create_userdata(L, room.lock(), room_metatable);
        }

        int to_lua(lua_State* L, IRoom::AlternateMode mode)
        {
            lua_pushstring(L, to_string(mode).c_str());
            return 1;
        }
    }
}
