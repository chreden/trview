#include "trview.h"
#include "../Lua.h"
#include "../Elements/Level/Lua_Level.h"
#include "../Elements/Room/Lua_Room.h"
#include "../Elements/Item/Lua_Item.h"
#include "../Elements/Trigger/Lua_Trigger.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            IApplication* current_application = nullptr;
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

            int trview_select_item(lua_State* L)
            {
                if (lua_istable(L, 1))
                {
                    lua_getfield(L, -1, "number");
                    int number = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    current_application->select_item(number);
                }
                return 0;
            }
        }

        void set_application(IApplication * application)
        {
            current_application = application;
        }

        void set_current_level(ILevel* level)
        {
            current_level = level;
            level_set_current_level(level);
            room_set_current_level(level);
            item_set_current_level(level);
            trigger_set_current_level(level);
        }

        void trview_register(lua_State * L)
        {
            constexpr struct luaL_Reg trview_lib[] =
            {
                { "__index", trview_index },
                { "select_item", trview_select_item },
                { NULL, NULL },
            };

            luaL_newlib(L, trview_lib);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
            lua_setglobal(L, "trview");
        }
    }
}
