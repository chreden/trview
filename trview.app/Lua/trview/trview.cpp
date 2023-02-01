#include "trview.h"
#include "../Elements/Level/Lua_Level.h"
#include "../../Application.h"
#include <trlevel/LevelEncryptedException.h>
#include "../../UserCancelledException.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int trview_load(lua_State* L)
            {
                IApplication* application = *static_cast<IApplication**>(lua_touserdata(L, 1));

                luaL_checktype(L, -1, LUA_TSTRING);
                const char* filename = lua_tostring(L, -1);

                try
                {
                    return create_level(L, application->load(filename));
                }
                catch (trlevel::LevelEncryptedException&)
                {
                    luaL_error(L, "Level is encrypted and cannot be loaded (%s)", filename);
                }
                catch (...)
                {
                    luaL_error(L, "Failed to load level (%s)", filename);
                }
                return 0;
            }

            int trview_index(lua_State* L)
            {
                IApplication* application = *static_cast<IApplication**>(lua_touserdata(L, 1));

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
                return 0;
            }

            int trview_newindex(lua_State* L)
            {
                IApplication* application = *static_cast<IApplication**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "level")
                {
                    if (auto level = to_level(L, -1))
                    {
                        try
                        {
                            application->set_current_level(level, ILevel::OpenMode::Full, true);
                        }
                        catch (trlevel::LevelEncryptedException&)
                        {
                            luaL_error(L, "Level is encrypted and cannot be loaded");
                        }
                        catch (UserCancelledException&)
                        {
                            luaL_error(L, "User cancelled level loading");
                        }
                        catch (...)
                        {
                            luaL_error(L, "Failed to load level");
                        }
                    }
                }
                return 0;
            }
        }

        void trview_register(lua_State* L, IApplication* application)
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
        }
    }
}
