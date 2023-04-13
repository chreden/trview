#include "Lua_Route.h"
#include "../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<IRoute**, std::shared_ptr<IRoute>> routes;

            int route_index(lua_State* L)
            {
                auto route = lua::get_self<IRoute>(L);
                const std::string key = lua_tostring(L, 2);

                route;
                key;

                return 0;
            }

            int route_newindex(lua_State* L)
            {
                auto route = lua::get_self<IRoute>(L);
                const std::string key = lua_tostring(L, 2);

                route;
                key;

                return 0;
            }

            int route_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                IRoute** userdata = static_cast<IRoute**>(lua_touserdata(L, 1));
                routes.erase(userdata);
                return 0;
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
    }
}
