#include "Lua_Route.h"
#include "../Lua.h"
#include "../../Routing/Route.h"
#include "../Elements/Item/Lua_Item.h"
#include "../Elements/Trigger/Lua_Trigger.h"
#include "../Elements/Room/Lua_Room.h"
#include "../Elements/Level/Lua_Level.h"
#include "Lua_Waypoint.h"
#include "../Vector3.h"
#include "../../Elements/ITrigger.h"
#include "../Colour.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<IRoute**, std::shared_ptr<IRoute>> routes;
            IRoute::Source route_source;

            int route_add(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                return create_waypoint(L, route->add(to_waypoint(L, 2)));
            }

            int route_remove(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                route->remove(to_waypoint(L, 2));
                return 0;
            }

            int route_clear(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                route->clear();
                return 0;
            }

            int route_index(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "add")
                {
                    lua_pushcfunction(L, route_add);
                    return 1;
                }
                else if (key == "colour")
                {
                    return create_colour(L, route->colour());
                }
                else if (key == "clear")
                {
                    lua_pushcfunction(L, route_clear);
                    return 1;
                }
                else if (key == "level")
                {
                    return create_level(L, route->level().lock());
                }
                else if (key == "remove")
                {
                    lua_pushcfunction(L, route_remove);
                    return 1;
                }
                else if (key == "waypoint_colour")
                {
                    return create_colour(L, route->waypoint_colour());
                }
                else if (key == "waypoints")
                {
                    std::vector<std::weak_ptr<IWaypoint>> waypoints;
                    for (uint32_t i = 0; i < route->waypoints(); ++i)
                    {
                        waypoints.push_back(route->waypoint(i));
                    }
                    return push_list_p(L, waypoints, create_waypoint);
                }

                return 0;
            }

            int route_newindex(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "colour")
                {
                    route->set_colour(to_colour(L, 3));
                }
                else if (key == "level")
                {
                    route->set_level(to_level(L, 3));
                }
                else if (key == "waypoint_colour")
                {
                    route->set_waypoint_colour(to_colour(L, 3));
                }
                else if (key == "waypoints")
                {
                    luaL_checktype(L, 3, LUA_TTABLE);

                    std::vector<std::weak_ptr<IWaypoint>> waypoints;

                    lua_pushnil(L);
                    while (lua_next(L, 3) != 0)
                    {
                        waypoints.push_back(to_waypoint(L, -1));
                        lua_pop(L, 1);
                    }

                    route->clear();
                    for (const auto& waypoint : waypoints)
                    {
                        route->add(waypoint.lock());
                    }
                }

                return 0;
            }

            int route_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                IRoute** userdata = static_cast<IRoute**>(lua_touserdata(L, 1));
                routes.erase(userdata);
                return 0;
            }

            int route_new(lua_State* L)
            {
                return create_route(L, route_source());
            }
        }

        int create_route(lua_State* L, const std::shared_ptr<IRoute>& route)
        {
            if (!route)
            {
                lua_pushnil(L);
                return 1;
            }

            IRoute** userdata = static_cast<IRoute**>(lua_newuserdata(L, sizeof(route.get())));
            *userdata = route.get();
            routes[userdata] = route;

            lua_newtable(L);
            lua_pushcfunction(L, route_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, route_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, route_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
            return 1;
        }

        std::shared_ptr<IRoute> to_route(lua_State* L, int index)
        {
            luaL_checktype(L, index, LUA_TUSERDATA);
            IRoute** userdata = static_cast<IRoute**>(lua_touserdata(L, index));
            auto found = routes.find(userdata);
            if (found == routes.end())
            {
                return {};
            }
            return found->second;
        }

        void route_register(lua_State* L, const IRoute::Source& source)
        {
            route_source = source;

            lua_newtable(L);
            lua_pushcfunction(L, route_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Route");
        }
    }
}