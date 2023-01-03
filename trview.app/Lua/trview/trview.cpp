#include "trview.h"
#include "../Elements/Level/Lua_Level.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            ILevel* current_level = nullptr;

            int trview_index(lua_State* L)
            {
                const std::string key = lua_tostring(L, 2);
                if (key == "level")
                {
                    create_level(L, current_level);
                    return 1;
                }
                return 0;
            }
        }

        void trview_register(lua_State* L)
        {
            constexpr struct luaL_Reg trview_lib[] =
            {
                { "__index", trview_index },
                { NULL, NULL },
            };

            luaL_newlib(L, trview_lib);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
            lua_setglobal(L, "trview");
        }

        void set_current_level(ILevel* level)
        {
            current_level = level;
            level_set_current_level(level);
        }
    }
}
