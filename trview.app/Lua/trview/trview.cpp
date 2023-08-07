#include "trview.h"
#include "../Elements/Level/Lua_Level.h"
#include "../../Application.h"
#include <trlevel/LevelEncryptedException.h>
#include "../../UserCancelledException.h"
#include "../Elements/Sector/Lua_Sector.h"
#include "../Route/Lua_Route.h"
#include "../Route/Lua_Waypoint.h"
#include "../Colour.h"
#include "../Vector3.h"
#include "Lua/Lua.h"

#include <future>

namespace trview
{
    namespace lua
    {
        namespace
        {
            struct LoadRequest
            {
                std::future<std::shared_ptr<ILevel>> level;
            };
            std::mutex request_mutex;
            std::vector<std::unique_ptr<LoadRequest>> active_requests;

            int trview_yield_load(lua_State* L, int, lua_KContext context)
            {
                LoadRequest* request = reinterpret_cast<LoadRequest*>(context);
                if (request->level._Is_ready())
                {
                    auto level = request->level.get();
                    {
                        std::lock_guard lock{ request_mutex };
                        std::erase_if(active_requests, [=](const auto& r) { return r.get() == request; });
                    }
                    return create_level(L, level);
                }
                return lua_yieldk(L, 0, context, trview_yield_load);
            }

            int trview_load(lua_State* L)
            {
                auto application = lua::get_self_raw<IApplication>(L);

                luaL_checktype(L, -1, LUA_TSTRING);
                const char* filename = lua_tostring(L, -1);

                try
                {
                    auto request = std::make_unique<LoadRequest>();
                    std::string file = filename;
                    request->level = std::async(std::launch::async, [=]() { return application->load(file); });
                    auto p = request.get();
                    {
                        std::lock_guard lock{ request_mutex };
                        active_requests.push_back(std::move(request));
                    }
                    return lua_yieldk(L, 0, reinterpret_cast<lua_KContext>(p), trview_yield_load);
                }
                catch (trlevel::LevelEncryptedException&)
                {
                    return luaL_error(L, "Level is encrypted and cannot be loaded (%s)", "");
                }
                catch (std::exception&)
                {
                    return luaL_error(L, "Failed to load level (%s)", "");
                }
            }

            int trview_index(lua_State* L)
            {
                auto application = lua::get_self_raw<IApplication>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "level")
                {
                    return create_level(L, application->current_level().lock());
                }
                else if (key == "load")
                {
                    lua_pushcfunction(L, trview_load);
                    return 1;
                }
                else if (key == "local_levels")
                {
                    return push_list(L, application->local_levels(), push_string);
                }
                else if (key == "recent_files")
                {
                    return push_list(L, application->settings().recent_files, push_string);
                }
                else if (key == "route")
                {
                    return create_route(L, application->route());
                }
                return 0;
            }

            int trview_newindex(lua_State* L)
            {
                auto application = lua::get_self_raw<IApplication>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "level")
                {
                    if (auto level = to_level(L, -1))
                    {
                        try
                        {
                            application->set_current_level(level, ILevel::OpenMode::Full, true);
                        }
                        catch (UserCancelledException&)
                        {
                            return luaL_error(L, "User cancelled level loading");
                        }
                        catch (...)
                        {
                            return luaL_error(L, "Failed to load level");
                        }
                    }
                }
                else if (key == "route")
                {
                    application->set_route(to_route(L, -1));
                }
                return 0;
            }
        }

        void trview_register(lua_State* L, IApplication* application,
            const IRoute::Source& route_source,
            const IRandomizerRoute::Source& randomizer_route_source,
            const IWaypoint::Source& waypoint_source,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files)
        {
            IApplication** userdata = static_cast<IApplication**>(lua_newuserdata(L, sizeof(application)));
            *userdata = application;

            lua_newtable(L);
            lua_pushcfunction(L, trview_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, trview_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_setmetatable(L, -2);
            lua_setglobal(L, "trview");

            sector_register(L);
            route_register(L, route_source, randomizer_route_source, dialogs, files);
            waypoint_register(L, waypoint_source);
            colour_register(L);
            vector3_register(L);
        }

        void set_settings(const UserSettings& settings)
        {
            waypoint_set_settings(settings);
            route_set_settings(settings);
        }
    }
}
