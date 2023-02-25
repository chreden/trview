#include "Lua_Sector.h"
#include "../../../Elements/ILevel.h"
#include "../Item/Lua_Item.h"
#include "../Trigger/Lua_Trigger.h"
#include "../Room/Lua_Room.h"
#include "../../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<ISector**, std::shared_ptr<ISector>> sectors;

            int sector_hasflag(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);

                luaL_checktype(L, -1, LUA_TNUMBER);
                long long flags = lua_tointeger(L, -1);

                return static_cast<long long>(sector->flags()) & flags;
            }

            int sector_index(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "flags")
                {
                    lua_pushinteger(L, static_cast<int>(sector->flags()));
                    return 1;
                }
                else if (key == "floordata")
                {
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "has_flag")
                {
                    lua_pushcfunction(L, sector_hasflag);
                    return 1;
                }
                else if (key == "number")
                {
                    lua_pushinteger(L, sector->id());
                    return 1;
                }
                else if (key == "portal")
                {
                    if (sector->is_portal())
                    {
                        if (auto room = sector->room().lock())
                        {
                            if (auto level = room->level().lock())
                            {
                                return create_room(L, level->room(sector->portal()).lock());
                            }
                        }
                    }
                    lua_pushnil(L);
                }
                else if (key == "room")
                {
                    return create_room(L, sector->room().lock());
                }
                else if (key == "room_above")
                {
                    if (sector->room_above() != 0xff)
                    {
                        if (auto room = sector->room().lock())
                        {
                            if (auto level = room->level().lock())
                            {
                                return create_room(L, level->room(sector->room_above()).lock());
                            }
                        }
                    }
                    lua_pushnil(L);
                }
                else if (key == "room_below")
                {
                    if (sector->room_below() != 0xff)
                    {
                        if (auto room = sector->room().lock())
                        {
                            if (auto level = room->level().lock())
                            {
                                return create_room(L, level->room(sector->room_below()).lock());
                            }
                        }
                    }
                    lua_pushnil(L);
                }
                else if (key == "trigger")
                {
                    return create_trigger(L, sector->trigger().lock());
                }
                else if (key == "x")
                {
                    lua_pushinteger(L, sector->x());
                    return 1;
                }
                else if (key == "z")
                {
                    lua_pushinteger(L, sector->z());
                    return 1;
                }

                return 0;
            }

            int sector_newindex(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);
                sector;
                return 0;
            }

            int sector_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                ISector** userdata = static_cast<ISector**>(lua_touserdata(L, 1));
                sectors.erase(userdata);
                return 0;
            }
        }

        int create_sector(lua_State* L, std::shared_ptr<ISector> sector)
        {
            if (!sector)
            {
                lua_pushnil(L);
                return 1;
            }

            ISector** userdata = static_cast<ISector**>(lua_newuserdata(L, sizeof(sector.get())));
            *userdata = sector.get();
            sectors[userdata] = sector;

            lua_newtable(L);
            lua_pushcfunction(L, sector_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, sector_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, sector_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
            return 1;
        }

        void sector_register(lua_State* L)
        {
            lua_newtable(L);
            create_enum<SectorFlag>(L, "Flags", 
            {
                { "None", SectorFlag::None },
                { "Portal", SectorFlag::Portal },
                { "Wall", SectorFlag::Wall },
                { "Trigger", SectorFlag::Trigger },
                { "Death", SectorFlag::Death },
                { "FloorSlant", SectorFlag::FloorSlant },
                { "CeilingSlant", SectorFlag::CeilingSlant },
                { "ClimbableNorth", SectorFlag::ClimbableNorth },
                { "ClimbableEast", SectorFlag::ClimbableEast },
                { "ClimbableSouth", SectorFlag::ClimbableSouth },
                { "ClimbableWest", SectorFlag::ClimbableWest },
                { "MonkeySwing", SectorFlag::MonkeySwing },
                { "RoomAbove", SectorFlag::RoomAbove },
                { "RoomBelow", SectorFlag::RoomBelow },
                { "MinecartLeft", SectorFlag::MinecartLeft },
                { "MinecartRight", SectorFlag::MinecartRight },
                { "SpecialWall", SectorFlag::SpecialWall },
                { "Climbable", SectorFlag::Climbable }
            });
            lua_setglobal(L, "Sector");
        }
    }
}
