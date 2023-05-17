#include "Lua_Waypoint.h"
#include "../Lua.h"
#include "../Colour.h"
#include "../Vector3.h"
#include "../Elements/Item/Lua_Item.h"
#include "../Elements/Trigger/Lua_Trigger.h"
#include "../Elements/Room/Lua_Room.h"
#include "../../Elements/ITrigger.h"
#include "../../Elements/IRoom.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<IWaypoint**, std::shared_ptr<IWaypoint>> waypoints;
            IWaypoint::Source waypoint_source;
            UserSettings user_settings;

            int waypoint_index(lua_State* L)
            {
                auto waypoint = get_self<IWaypoint>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "colour")
                {
                    return create_colour(L, waypoint->route_colour());
                }
                else if (key == "notes")
                {
                    lua_pushstring(L, waypoint->notes().c_str());
                    return 1;
                }
                else if (key == "randomizer_settings")
                {
                    auto settings = waypoint->randomizer_settings();
                    lua_newtable(L);

                    for (const auto& setting : user_settings.randomizer.settings)
                    {
                        const auto value_to_set =
                            settings.find(setting.first) == settings.end() ?
                            setting.second.default_value : settings[setting.first];

                        switch (setting.second.type)
                        {
                            case RandomizerSettings::Setting::Type::Boolean:
                            {
                                lua_pushboolean(L, std::get<bool>(value_to_set));
                                break;
                            }
                            case RandomizerSettings::Setting::Type::String:
                            {
                                lua_pushstring(L, std::get<std::string>(value_to_set).c_str());
                                break;
                            }
                            case RandomizerSettings::Setting::Type::Number:
                            {
                                lua_pushnumber(L, std::get<float>(value_to_set));
                                break;
                            }
                        }
                        lua_setfield(L, -2, setting.first.c_str());
                    }
                    return 1;
                }
                else if (key == "waypoint_colour")
                {
                    return create_colour(L, waypoint->waypoint_colour());
                }

                return 0;
            }

            int waypoint_newindex(lua_State* L)
            {
                auto waypoint = get_self<IWaypoint>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "colour")
                {
                    waypoint->set_route_colour(to_colour(L, 3));
                }
                else if (key == "notes")
                {
                    luaL_checkstring(L, 3);
                    waypoint->set_notes(lua_tostring(L, 3));
                }
                else if (key == "randomizer_settings")
                {
                    luaL_checktype(L, LUA_TTABLE, 3);

                }
                else if (key == "waypoint_colour")
                {
                    waypoint->set_waypoint_colour(to_colour(L, 3));
                }

                return 0;
            }

            int waypoint_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                IWaypoint** userdata = static_cast<IWaypoint**>(lua_touserdata(L, 1));
                waypoints.erase(userdata);
                return 0;
            }

            int waypoint_new(lua_State* L)
            {
                const Vector3 normal = to_vector3(L, 1, "normal", Vector3::Down);

                if (LUA_TUSERDATA == lua_getfield(L, 1, "item"))
                {
                    if (auto item = to_item(L, -1))
                    {
                        return create_waypoint(L,
                            waypoint_source(
                                item->position(),
                                normal,
                                item->room(),
                                IWaypoint::Type::Entity,
                                item->number(),
                                Colour::White,
                                Colour::White));
                    }
                    return 0;
                }
                else
                {
                    lua_pop(L, 1);
                }

                if (LUA_TUSERDATA == lua_getfield(L, 1, "trigger"))
                {
                    if (auto trigger = to_trigger(L, -1))
                    {
                        return create_waypoint(L,
                            waypoint_source(
                                trigger->position(),
                                normal,
                                trigger->room(),
                                IWaypoint::Type::Entity,
                                trigger->number(),
                                Colour::White,
                                Colour::White));
                    }
                    return 0;
                }
                else
                {
                    lua_pop(L, 1);
                }

                if (LUA_TTABLE == lua_getfield(L, 1, "position"))
                {
                    Vector3 position = to_vector3(L, -1);
                    if (auto room = to_room(L, 2, "room"))
                    {
                        return create_waypoint(L,
                            waypoint_source(
                                position,
                                normal,
                                room->number(),
                                IWaypoint::Type::Position,
                                0,
                                Colour::White,
                                Colour::White));
                    }
                    else
                    {
                        luaL_error(L, "Room must be valid when using Position in Waypoint.new");
                    }
                    return 0;
                }

                luaL_error(L, "One of item, trigger or position must be specified for Waypoint.new");
                return 0;
            }
        }

        void waypoint_register(lua_State* L, IWaypoint::Source source)
        {
            waypoint_source = source;

            lua_newtable(L);
            lua_pushcfunction(L, waypoint_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Waypoint");
        }

        std::shared_ptr<IWaypoint> to_waypoint(lua_State* L, int index)
        {
            luaL_checktype(L, index, LUA_TUSERDATA);
            IWaypoint** userdata = static_cast<IWaypoint**>(lua_touserdata(L, index));
            auto found = waypoints.find(userdata);
            if (found == waypoints.end())
            {
                return {};
            }
            return found->second;
        }

        int create_waypoint(lua_State* L, const std::shared_ptr<IWaypoint>& waypoint)
        {
            if (!waypoint)
            {
                lua_pushnil(L);
                return 1;
            }

            IWaypoint** userdata = static_cast<IWaypoint**>(lua_newuserdata(L, sizeof(waypoint.get())));
            *userdata = waypoint.get();
            waypoints[userdata] = waypoint;

            lua_newtable(L);
            lua_pushcfunction(L, waypoint_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, waypoint_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, waypoint_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
            return 1;
        }

        void waypoint_set_settings(const UserSettings& new_settings)
        {
            user_settings = new_settings;
        }
    }
}
