#include "Lua_Sector.h"
#include "../../../Elements/ILevel.h"
#include "../../../Elements/IRoom.h"
#include "../../../Elements/ISector.h"
#include "../../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            ILevel* current_level = nullptr;
        }

        void create_sector(lua_State* L, const std::weak_ptr<ISector>& sector)
        {
            const auto sector_ptr = sector.lock();
            if (!sector_ptr)
            {
                lua_pushnil(L);
                return;
            }

            lua_newtable(L);
            set_integer(L, "x", sector_ptr->x());
            set_integer(L, "z", sector_ptr->z());
            set_integer(L, "flags", static_cast<int>(sector_ptr->flags()));
        }

        void sector_set_current_level(ILevel* level)
        {
            current_level = level;
        }
    }
}
