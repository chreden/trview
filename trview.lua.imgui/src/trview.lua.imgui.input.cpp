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
                const auto label = get_string(L, 1, "label");
                double out = get_double(L, 1, "value");
                const auto step = get_optional_double(L, 1, "step");
                const auto step_fast = get_optional_double(L, 1, "step_fast");
                const auto format = get_optional_string(L, 1, "format");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputDouble(label.c_str(), &out, step.value_or(0.0), step_fast.value_or(0.0), format.value_or("%.6f").c_str(), flags.value_or(ImGuiInputTextFlags_None));
                lua_pushnumber(L, result);
                lua_pushnumber(L, out);
                return 2;
            }

            int input_float(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                float out = get_float(L, 1, "value");
                const auto step = get_optional_float(L, 1, "step");
                const auto step_fast = get_optional_float(L, 1, "step_fast");
                const auto format = get_optional_string(L, 1, "format");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputFloat(label.c_str(), &out, step.value_or(0.0f), step_fast.value_or(0.0f), format.value_or("%.3f").c_str(), flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushnumber(L, out);
                return 2;
            }

            int input_float2(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                float values[2]
                {
                    get_float(L, 1, "value1"),
                    get_float(L, 1, "value2")
                };
                const auto format = get_optional_string(L, 1, "format");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputFloat2(label.c_str(), values, format.value_or("%.3f").c_str(), flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushnumber(L, values[0]);
                lua_pushnumber(L, values[1]);
                return 3;
            }

            int input_float3(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                float values[3]
                {
                    get_float(L, 1, "value1"),
                    get_float(L, 1, "value2"),
                    get_float(L, 1, "value3")
                };
                const auto format = get_optional_string(L, 1, "format");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputFloat3(label.c_str(), values, format.value_or("%.3f").c_str(), flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushnumber(L, values[0]);
                lua_pushnumber(L, values[1]);
                lua_pushnumber(L, values[2]);
                return 4;
            }

            int input_float4(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                float values[4]
                {
                    get_float(L, 1, "value1"),
                    get_float(L, 1, "value2"),
                    get_float(L, 1, "value3"),
                    get_float(L, 1, "value4")
                };
                const auto format = get_optional_string(L, 1, "format");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputFloat4(label.c_str(), values, format.value_or("%.3f").c_str(), flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushnumber(L, values[0]);
                lua_pushnumber(L, values[1]);
                lua_pushnumber(L, values[2]);
                lua_pushnumber(L, values[3]);
                return 5;
            }

            int input_int(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                int value = get_integer(L, 1, "value");
                const auto step = get_optional_integer(L, 1, "step");
                const auto step_fast = get_optional_integer(L, 1, "step_fast");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputInt(label.c_str(), &value, step.value_or(1), step_fast.value_or(100), flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushinteger(L, value);
                return 2;
            }

            int input_int2(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                int values[2]
                {
                    get_integer(L, 1, "value1"),
                    get_integer(L, 1, "value2")
                };
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputInt2(label.c_str(), values, flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushinteger(L, values[0]);
                lua_pushinteger(L, values[1]);
                return 3;
            }

            int input_int3(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                int values[3]
                {
                    get_integer(L, 1, "value1"),
                    get_integer(L, 1, "value2"),
                    get_integer(L, 1, "value3")
                };
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputInt3(label.c_str(), values, flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushinteger(L, values[0]);
                lua_pushinteger(L, values[1]);
                lua_pushinteger(L, values[2]);
                return 4;
            }

            int input_int4(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                int values[4]
                {
                    get_integer(L, 1, "value1"),
                    get_integer(L, 1, "value2"),
                    get_integer(L, 1, "value3"),
                    get_integer(L, 1, "value4")
                };
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputInt4(label.c_str(), values, flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushinteger(L, values[0]);
                lua_pushinteger(L, values[1]);
                lua_pushinteger(L, values[2]);
                lua_pushinteger(L, values[3]);
                return 5;
            }

            int input_scalar(lua_State* L)
            {
                return luaL_error(L, "input_scalar not yet implemented");
            }

            int input_scalarN(lua_State* L)
            {
                return luaL_error(L, "input_scalarN not yet implemented");
            }

            int input_text(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                std::string out = get_string(L, 1, "value");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::InputText(label.c_str(), &out, flags.value_or(ImGuiInputTextFlags_None));
                lua_pushboolean(L, result);
                lua_pushstring(L, out.c_str());
                return 2;
            }
        }

        void register_input(lua_State* L)
        {
            // Callbacks are not yet implemented.
            set_enum(L, "InputTextFlags", -1,
                {
                    { "None", ImGuiInputTextFlags_None },
                    { "CharsDecimal", ImGuiInputTextFlags_CharsDecimal },
                    { "CharsHexadecimal", ImGuiInputTextFlags_CharsHexadecimal },
                    { "CharsUppercase", ImGuiInputTextFlags_CharsUppercase },
                    { "CharsNoBlank", ImGuiInputTextFlags_CharsNoBlank },
                    { "AutoSelectAll", ImGuiInputTextFlags_AutoSelectAll },
                    { "EnterReturnsTrue", ImGuiInputTextFlags_EnterReturnsTrue },
                    { "CallbackCompletion", ImGuiInputTextFlags_CallbackCompletion },
                    { "CallbackHistory", ImGuiInputTextFlags_CallbackHistory },
                    { "CallbackAlways", ImGuiInputTextFlags_CallbackAlways },
                    { "CallbackCharFilter", ImGuiInputTextFlags_CallbackCharFilter },
                    { "AllowTabInput", ImGuiInputTextFlags_AllowTabInput },
                    { "CtrlEnterForNewLine", ImGuiInputTextFlags_CtrlEnterForNewLine },
                    { "NoHorizontalScroll", ImGuiInputTextFlags_NoHorizontalScroll },
                    { "AlwaysOverwrite", ImGuiInputTextFlags_AlwaysOverwrite },
                    { "ReadOnly", ImGuiInputTextFlags_ReadOnly },
                    { "Password", ImGuiInputTextFlags_Password },
                    { "NoUndoRedo", ImGuiInputTextFlags_NoUndoRedo },
                    { "CharsScientific", ImGuiInputTextFlags_CharsScientific },
                    { "CallbackResize", ImGuiInputTextFlags_CallbackResize },
                    { "CallbackEdit", ImGuiInputTextFlags_CallbackEdit }
                });

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
                // { "InputScalar", input_scalar },
                // { "InputScalarN", input_scalarN },
                { "InputText", input_text },
                { nullptr, nullptr }
            };
            luaL_setfuncs(L, funcs, 0);
        }
    }
}
