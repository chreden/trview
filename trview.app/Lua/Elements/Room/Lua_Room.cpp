#include "Lua_Room.h"
#include "../../../Elements/ILevel.h"
#include "../Item/Lua_Item.h"
#include "../Trigger/Lua_Trigger.h"
#include "../Sector/Lua_Sector.h"
#include "../CameraSink/Lua_CameraSink.h"
#include "../Light/Lua_Light.h"
#include "../../Lua.h"
#include "../Level/Lua_Level.h"
#include "../StaticMesh/Lua_StaticMesh.h"
#include "../../Vector3.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int get_sector(lua_State* L)
            {
                const auto room = lua::get_self<IRoom>(L);
                const auto x = static_cast<int32_t>(lua_tointeger(L, 2) - 1);
                const auto z = static_cast<int32_t>(lua_tointeger(L, 3) - 1);
                return create_sector(L, room->sector(x, z).lock());
            }

            int room_hasflag(lua_State* L)
            {
                auto room = lua::get_self<IRoom>(L);

                luaL_checktype(L, -1, LUA_TNUMBER);
                long long flags = lua_tointeger(L, -1);

                lua_pushboolean(L, static_cast<long long>(room->flags()) & flags);
                return 1;
            }

            int room_index(lua_State* L)
            {
                auto room = lua::get_self<IRoom>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "alternate_mode")
                {
                    lua_pushstring(L, to_string(room->alternate_mode()).c_str());
                    return 1;
                }
                else if (key == "alternate_group")
                {
                    lua_pushinteger(L, room->alternate_group());
                    return 1;
                }
                else if (key == "alternate_room")
                {
                    if (auto level = room->level().lock())
                    {
                        return create_room(L, level->room(room->alternate_room()).lock());
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "cameras_and_sinks")
                {
                    return push_list_p(L, room->camera_sinks(), create_camera_sink);
                }
                else if (key == "flags")
                {
                    lua_pushinteger(L, room->flags());
                    return 1;
                }
                else if (key == "has_flag")
                {
                    lua_pushcfunction(L, room_hasflag);
                    return 1;
                }
                else if (key == "items")
                {
                    return push_list_p(L, room->items(), create_item);
                }
                else if (key == "level")
                {
                    return create_level(L, room->level().lock());
                }
                else if (key == "lights")
                {
                    return push_list_p(L, room->lights(), create_light);
                }
                else if (key == "number")
                {
                    lua_pushinteger(L, room->number());
                    return 1;
                }
                else if (key == "num_x_sectors")
                {
                    lua_pushinteger(L, room->num_x_sectors());
                    return 1;
                }
                else if (key == "num_z_sectors")
                {
                    lua_pushinteger(L, room->num_z_sectors());
                    return 1;
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
                else if (key == "sectors")
                {
                    return push_list(L, room->sectors(), create_sector);
                }
                else if (key == "static_meshes")
                {
                    return push_list_p(L, room->static_meshes(), create_static_mesh);
                }
                else if (key == "triggers")
                {
                    return push_list_p(L, room->triggers(), create_trigger);
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, room->visible());
                    return 1;
                }

                return 0;
            }

            int room_newindex(lua_State* L)
            {
                auto room = lua::get_self<IRoom>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    room->set_visible(lua_toboolean(L, -1));
                }

                return 0;
            }
        }

        int create_room(lua_State* L, std::shared_ptr<IRoom> room)
        {
            return create(L, room, room_index, room_newindex);
        }

        void room_register(lua_State* L)
        {
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
            return get_self<IRoom>(L, index);
        }

        std::shared_ptr<IRoom> to_room(lua_State* L, int index, const std::string& field_name)
        {
            lua_getfield(L, index, field_name.c_str());
            auto room = to_room(L, -1);
            lua_pop(L, 1);
            return room;
        }
    }
}
