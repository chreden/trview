#include "Lua_Route.h"
#include "../Lua.h"
#include "../../Routing/IRoute.h"
#include "../../Routing/IRandomizerRoute.h"
#include "../Elements/Item/Lua_Item.h"
#include "../Elements/Trigger/Lua_Trigger.h"
#include "../Elements/Room/Lua_Room.h"
#include "../Elements/Level/Lua_Level.h"
#include "../../Elements/ILevel.h"
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
            IRoute::Source route_source;
            IRandomizerRoute::Source randomizer_route_source;
            std::shared_ptr<IDialogs> dialogs;
            std::shared_ptr<IFiles> files;
            UserSettings user_settings;

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

            std::optional<std::string> route_save_as(const std::shared_ptr<IRoute>& route, const std::string& filename)
            {
                const bool is_rando = std::dynamic_pointer_cast<IRandomizerRoute>(route) != nullptr;
                if (!filename.empty())
                {
                    if (dialogs->message_box(
                        to_utf16(std::format("Allow script to save route as {}?", filename)),
                        L"Route export",
                        IDialogs::Buttons::Yes_No))
                    {
                        route->save_as(files, filename, user_settings);
                        return filename;
                    }
                }
                else
                {
                    std::vector<IDialogs::FileFilter> filters;
                    if (is_rando)
                    {
                        filters.push_back({ L"Randomizer Locations", { L"*.json" } });
                    }
                    else
                    {
                        filters.push_back({ L"trview route", { L"*.tvr" } });
                    }

                    if (const auto result = dialogs->save_file(L"Select location to save route as", filters, 1))
                    {
                        route->save_as(files, result->filename, user_settings);
                        return result->filename;
                    }
                }

                return std::nullopt;
            }

            int route_save_as(lua_State* L)
            {
                auto route = get_self<IRoute>(L);

                std::string filename;

                if (lua_type(L, 2) == LUA_TTABLE)
                {
                    if (LUA_TSTRING == lua_getfield(L, 2, "filename"))
                    {
                        filename = lua_tostring(L, -1);
                    }
                }

                route_save_as(route, filename);
                return 0;
            }

            int route_save(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                if (route->filename())
                {
                    route->save(files, user_settings);
                }
                else if (auto result = route_save_as(route, {}))
                {
                    route->set_filename(result.value());
                }
                return 0;
            }

            int route_reload(lua_State* L)
            {
                auto route = get_self<IRoute>(L);
                route->reload(files, user_settings);
                return 0;
            }

            int route_open(lua_State* L)
            {
                std::string filename;
                bool is_rando = false;
                std::shared_ptr<ILevel> level;

                if (LUA_TTABLE == lua_type(L, 1))
                {
                    if (LUA_TBOOLEAN == lua_getfield(L, 1, "is_randomizer"))
                    {
                        is_rando = lua_toboolean(L, -1);
                    }

                    if (LUA_TSTRING == lua_getfield(L, 1, "filename"))
                    {
                        filename = lua_tostring(L, -1);
                    }

                    if (LUA_TUSERDATA == lua_getfield(L, 1, "level"))
                    {
                        level = to_level(L, -1);
                    }
                }

                if (filename.empty())
                {
                    std::vector<IDialogs::FileFilter> filters{ { L"trview route", { L"*.tvr" } } };
                    if (user_settings.randomizer_tools || is_rando)
                    {
                        filters.push_back({ L"Randomizer Locations", { L"*.json" } });
                    }
                    if (auto result = dialogs->open_file(L"Select route to import", filters, OFN_FILEMUSTEXIST))
                    {
                        is_rando = result->filter_index == 2 || result->filename.ends_with(".json");
                        filename = result->filename;
                    }
                    else
                    {
                        lua_pushnil(L);
                        return 1;
                    }
                }

                auto route = is_rando ?
                    import_randomizer_route(randomizer_route_source, files, filename, user_settings.randomizer) :
                    import_route(route_source, files, filename);
                if (route)
                {
                    route->set_level(level);
                }
                return create_route(L, route);
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
                else if (key == "is_randomizer")
                {
                    lua_pushboolean(L, std::dynamic_pointer_cast<IRandomizerRoute>(route) != nullptr);
                    return 1;
                }
                else if (key == "level")
                {
                    return create_level(L, route->level().lock());
                }
                else if (key == "reload")
                {
                    lua_pushcfunction(L, route_reload);
                    return 1;
                }
                else if (key == "remove")
                {
                    lua_pushcfunction(L, route_remove);
                    return 1;
                }
                else if (key == "save")
                {
                    lua_pushcfunction(L, route_save);
                    return 1;
                }
                else if (key == "save_as")
                {
                    lua_pushcfunction(L, route_save_as);
                    return 1;
                }
                else if (key == "show_route_line")
                {
                    lua_pushboolean(L, route->show_route_line());
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
                else if (key == "show_route_line")
                {
                    route->set_show_route_line(lua_toboolean(L, 3));
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

            int route_new(lua_State* L)
            {
                if (lua_gettop(L) != 0 && lua_type(L, 1) == LUA_TTABLE)
                {
                    if (LUA_TBOOLEAN == lua_getfield(L, 1, "is_randomizer"))
                    {
                        if (lua_toboolean(L, -1))
                        {
                            return create_route(L, randomizer_route_source(std::nullopt));
                        }
                    }
                }
                return create_route(L, route_source(std::nullopt));
            }
        }

        int create_route(lua_State* L, const std::shared_ptr<IRoute>& route)
        {
            return create(L, route, route_index, route_newindex);
        }

        std::shared_ptr<IRoute> to_route(lua_State* L, int index)
        {
            return get_self<IRoute>(L, index);
        }

        void route_register(lua_State* L, const IRoute::Source& source, const IRandomizerRoute::Source& randomizer_source, const std::shared_ptr<IDialogs>& dialogs_, const std::shared_ptr<IFiles>& files_)
        {
            route_source = source;
            randomizer_route_source = randomizer_source;
            dialogs = dialogs_;
            files = files_;

            lua_newtable(L);
            lua_pushcfunction(L, route_open);
            lua_setfield(L, -2, "open");
            lua_pushcfunction(L, route_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Route");
        }

        void route_set_settings(const UserSettings& new_settings)
        {
            user_settings = new_settings;
        }
    }
}
