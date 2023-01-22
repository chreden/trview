#include "trview.h"
#include "../Elements/Level/Lua_Level.h"
#include "../../Application.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int trview_index(lua_State* L)
            {
                IApplication* application = *static_cast<IApplication**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "level")
                {
                    create_level(L, application->current_level().lock());
                    return 1;
                }
                return 0;
            }

            int trview_newindex(lua_State*)
            {
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
