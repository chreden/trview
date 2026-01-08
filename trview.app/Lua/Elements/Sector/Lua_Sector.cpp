#include "Lua_Sector.h"
#include "../../../Elements/ILevel.h"
#include "../Item/Lua_Item.h"
#include "../Trigger/Lua_Trigger.h"
#include "../Room/Lua_Room.h"
#include "../../Lua.h"
#include <trview.common/Algorithms.h>
#include "../../../Elements/Floordata.h"

#include <ranges>

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::array<int, 4> to_corner_clicks(const std::shared_ptr<ISector>& sector, const std::array<float, 4>& corners)
            {
                float base = 0;
                if (auto room = sector->room().lock())
                {
                    base = room->info().yBottom / trlevel::Scale;
                }
                return
                {
                    static_cast<int>((corners[0] - base) / -0.25f),
                    static_cast<int>((corners[1] - base) / -0.25f),
                    static_cast<int>((corners[2] - base) / -0.25f),
                    static_cast<int>((corners[3] - base) / -0.25f )
                };
            }

            std::array<int, 4> to_ceiling_corner_clicks(const std::shared_ptr<ISector>& sector, const std::array<float, 4>& corners)
            {
                float base = 0;
                if (auto room = sector->room().lock())
                {
                    base = room->info().yTop / trlevel::Scale;
                }
                return
                {
                    static_cast<int>((corners[0] - base) / 0.25f),
                    static_cast<int>((corners[1] - base) / 0.25f),
                    static_cast<int>((corners[2] - base) / 0.25f),
                    static_cast<int>((corners[3] - base) / 0.25f)
                };
            }

            int sector_hasflag(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);

                luaL_checktype(L, -1, LUA_TNUMBER);
                long long flags = lua_tointeger(L, -1);

                lua_pushboolean(L, static_cast<long long>(sector->flags()) & flags);
                return 1;
            }

            int sector_index(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "above")
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
                    return 1;
                }
                else if (key == "below")
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
                    return 1;
                }
                else if (key == "ceiling_corners")
                {
                    lua_newtable(L);
                    const auto corners = to_ceiling_corner_clicks(sector, sector->ceiling_corners());
                    lua_pushinteger(L, corners[0]);
                    lua_rawseti(L, -2, 1);
                    lua_pushinteger(L, corners[1]);
                    lua_rawseti(L, -2, 2);
                    lua_pushinteger(L, corners[2]);
                    lua_rawseti(L, -2, 3);
                    lua_pushinteger(L, corners[3]);
                    lua_rawseti(L, -2, 4);
                    return 1;
                }
                else if (key == "ceiling_triangulation")
                {
                    lua_pushstring(L, to_string(sector->ceiling_triangulation()).c_str());
                    return 1;
                }
                else if (key == "corners")
                {
                    lua_newtable(L);
                    const auto corners = to_corner_clicks(sector, sector->corners());
                    lua_pushinteger(L, corners[0]);
                    lua_rawseti(L, -2, 1);
                    lua_pushinteger(L, corners[1]);
                    lua_rawseti(L, -2, 2);
                    lua_pushinteger(L, corners[2]);
                    lua_rawseti(L, -2, 3);
                    lua_pushinteger(L, corners[3]);
                    lua_rawseti(L, -2, 4);
                    return 1;
                }
                else if (key == "flags")
                {
                    lua_pushinteger(L, static_cast<int>(sector->flags()));
                    return 1;
                }
                else if (key == "floordata")
                {
                    if (auto room = sector->room().lock())
                    {
                        if (auto level = room->level().lock())
                        {
                            const auto data = level->floor_data();
                            if (sector->floordata_index() < data.size())
                            {
                                lua_newtable(L);
                                push_list(L, 
                                    parse_floordata(level->floor_data(), sector->floordata_index(), FloordataMeanings::None, level->trng(), level->platform_and_version()).commands
                                    | std::views::transform([](auto& f) { return f.data; })
                                    | std::views::join,
                                    [](auto L, auto f) { lua_pushinteger(L, f); });
                                return 1;
                            }
                        }
                    }
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
                                return create_room(L, level->room(sector->portals()[0]).lock());
                            }
                        }
                    }
                    lua_pushnil(L);
                }
                else if (key == "portals")
                {
                    if (sector->is_portal())
                    {
                        if (auto room = sector->room().lock())
                        {
                            if (auto level = room->level().lock())
                            {
                                lua_newtable(L);
                                push_list(L,
                                    sector->portals(),
                                    [&](auto L, auto f) { create_room(L, level->room(f).lock()); });
                                return 1;
                            }
                        }
                    }
                    lua_pushnil(L);
                }
                else if (key == "room")
                {
                    return create_room(L, sector->room().lock());
                }
                else if (key == "sector_above")
                {
                    return create_sector(L, sector_above(sector).value_or({}).sector);
                }
                else if (key == "sector_below")
                {
                    return create_sector(L, sector_below(sector).value_or({}).sector);
                }
                else if (key == "tilt_x")
                {
                    lua_pushnumber(L, sector->tilt_x());
                    return 1;
                }
                else if (key == "tilt_z")
                {
                    lua_pushnumber(L, sector->tilt_z());
                    return 1;
                }
                else if (key == "triangulation")
                {
                    lua_pushstring(L, to_string(sector->triangulation()).c_str());
                    return 1;
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
        }

        int create_sector(lua_State* L, std::shared_ptr<ISector> sector)
        {
            if (!sector)
            {
                lua_pushnil(L);
                return 1;
            }

            set_self(L, sector);

            lua_newtable(L);
            lua_pushcfunction(L, sector_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, sector_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, gc<ISector>);
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

        std::shared_ptr<ISector> to_sector(lua_State* L, int index)
        {
            luaL_checktype(L, index, LUA_TUSERDATA);
            std::shared_ptr<ISector>* userdata = static_cast<std::shared_ptr<ISector>*>(lua_touserdata(L, index));
            return *userdata;
        }
    }
}
