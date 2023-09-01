#include "pch.h"
#include "trview.lua.imgui.input.h"
#include "tables.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int input_int(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                int out = 0;
                bool result = ImGui::InputInt(label.c_str(), &out);
                lua_pushboolean(L, result);
                lua_pushinteger(L, out);
                return 2;
            }

            int input_text(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                std::string out;
                bool result = ImGui::InputText(label.c_str(), &out);
                lua_pushboolean(L, result);
                lua_pushstring(L, out.c_str());
                return 2;
            }
        }

        void register_input(lua_State* L)
        {
            constexpr luaL_Reg funcs[] = {
                { "InputInt", input_int },
                { "InputText", input_text },
                { nullptr, nullptr }
            };
            luaL_setfuncs(L, funcs, 0);
        }
    }
}
