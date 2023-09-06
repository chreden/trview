#include "pch.h"
#include "trview.lua.imgui.input.h"
#include "tables.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int input_double(lua_State* L)
            {
                return 0;
            }

            int input_float(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                float out = 0.0f;
                bool result = ImGui::InputFloat(label.c_str(), &out);
                lua_pushboolean(L, result);
                lua_pushnumber(L, out);
                return 2;
            }

            int input_float2(lua_State* L)
            {
                return 0;
            }

            int input_float3(lua_State* L)
            {
                return 0;
            }

            int input_float4(lua_State* L)
            {
                return 0;
            }

            int input_int(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                int out = 0;
                bool result = ImGui::InputInt(label.c_str(), &out);
                lua_pushboolean(L, result);
                lua_pushinteger(L, out);
                return 2;
            }

            int input_int2(lua_State* L)
            {
                return 0;
            }

            int input_int3(lua_State* L)
            {
                return 0;
            }

            int input_int4(lua_State* L)
            {
                return 0;
            }

            int input_scalar(lua_State* L)
            {
                return 0;
            }

            int input_scalarN(lua_State* L)
            {
                return 0;
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
                { "InputDouble", input_double },
                { "InputFloat", input_float },
                { "InputFloat2", input_float2 },
                { "InputFloat3", input_float3 },
                { "InputFloat4", input_float4 },
                { "InputInt", input_int },
                { "InputInt2", input_int2 },
                { "InputInt3", input_int3 },
                { "InputInt4", input_int4 },
                { "InputScalar", input_scalar },
                { "InputScalarN", input_scalarN },
                { "InputText", input_text },
                { nullptr, nullptr }
            };
            luaL_setfuncs(L, funcs, 0);
        }
    }
}
