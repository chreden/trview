// trview.lua.imgui.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "trview.lua.imgui.h"

#include <external/imgui/imgui.h>
#include <optional>
#include <string>

namespace trview
{
    namespace lua
    {
        namespace
        {
            /// <summary>
            /// Get an optional bool named field from a table.
            /// </summary>
            /// <param name="L">Lua state.</param>
            /// <param name="index">Index of the table on the stack.</param>
            /// <param name="name">Field name.</param>
            /// <returns>Optional - empty if not set, or boolean.</returns>
            std::optional<bool> get_optional_bool(lua_State* L, int index, const std::string& name)
            {
                luaL_checktype(L, index, LUA_TTABLE);
                lua_getfield(L, index, name.c_str());
                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);
                    return std::nullopt;
                }
                luaL_checktype(L, -1, LUA_TBOOLEAN);
                bool value = lua_toboolean(L, -1);
                lua_pop(L, 1);
                return value;
            }

            bool get_bool(lua_State* L, int index, const std::string& name)
            {
                luaL_checktype(L, index, LUA_TTABLE);
                lua_getfield(L, index, name.c_str());
                luaL_checktype(L, -1, LUA_TBOOLEAN);
                bool value = lua_toboolean(L, -1);
                lua_pop(L, 1);
                return value;
            }

            int get_integer(lua_State* L, int index, const std::string& name)
            {
                luaL_checktype(L, index, LUA_TTABLE);
                lua_getfield(L, index, name.c_str());
                luaL_checktype(L, -1, LUA_TNUMBER);
                int value = lua_tointeger(L, -1);
                lua_pop(L, 1);
                return value;
            }

            std::string get_string(lua_State* L, int index, const std::string& name)
            {
                luaL_checktype(L, index, LUA_TTABLE);
                lua_getfield(L, index, name.c_str());
                luaL_checktype(L, -1, LUA_TSTRING);
                std::string value = lua_tostring(L, -1);
                lua_pop(L, 1);
                return value;
            }

            int begin(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                const auto name = get_string(L, 1, "name");
                const auto open = get_optional_bool(L, 1, "open");
                bool is_open = open.value_or(true);
                bool result = ImGui::Begin(
                    name.c_str(),
                    open.has_value() ? &is_open : nullptr,
                    0);
                lua_pushboolean(L, result);
                lua_pushboolean(L, is_open);
                return 2;
            }

            int finish(lua_State* L)
            {
                ImGui::End();
                return 0;
            }

            int button(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                const auto label = get_string(L, 1, "label");
                bool result = ImGui::Button(label.c_str(), ImVec2(0, 0));
                lua_pushboolean(L, result);
                return 1;
            }

            int checkbox(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                const auto label = get_string(L, 1, "label");
                auto checked = get_bool(L, 1, "checked");
                bool result = ImGui::Checkbox(label.c_str(), &checked);
                
                lua_pushboolean(L, result);
                lua_pushboolean(L, checked);
                return 2;
            }

            int text(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                auto text = get_string(L, 1, "text");
                ImGui::Text(text.c_str());
                return 0;
            }

            int begin_table(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                auto id = get_string(L, 1, "id");
                auto column = get_integer(L, 1, "column");
                bool result = ImGui::BeginTable(id.c_str(), column);
                lua_pushboolean(L, result);
                return 1;
            }

            int end_table(lua_State* L)
            {
                ImGui::EndTable();
                return 0;
            }

            int table_setup_column(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                ImGui::TableSetupColumn(label.c_str());
                return 0;
            }

            int table_next_column(lua_State* L)
            {
                lua_pushboolean(L, ImGui::TableNextColumn());
                return 1;
            }

            int table_headers_row(lua_State* L)
            {
                ImGui::TableHeadersRow();
                return 0;
            }

            int table_setup_scroll_freeze(lua_State* L)
            {
                const auto cols = get_integer(L, 1, "cols");
                const auto rows = get_integer(L, 1, "rows");
                ImGui::TableSetupScrollFreeze(cols, rows);
                return 0;
            }
        }

        void imgui_register(lua_State* L)
        {
            lua_newtable(L);
            // Windows
            lua_pushcfunction(L, begin);
            lua_setfield(L, -2, "Begin");
            lua_pushcfunction(L, finish);
            lua_setfield(L, -2, "End");
            // Buttons
            lua_pushcfunction(L, button);
            lua_setfield(L, -2, "Button");
            // Checkbox
            lua_pushcfunction(L, checkbox);
            lua_setfield(L, -2, "Checkbox");
            // Text
            lua_pushcfunction(L, text);
            lua_setfield(L, -2, "Text");
            // Table
            lua_pushcfunction(L, begin_table);
            lua_setfield(L, -2, "BeginTable");
            lua_pushcfunction(L, end_table);
            lua_setfield(L, -2, "EndTable");
            lua_pushcfunction(L, table_next_column);
            lua_setfield(L, -2, "TableNextColumn");
            lua_pushcfunction(L, table_setup_column);
            lua_setfield(L, -2, "TableSetupColumn");
            lua_pushcfunction(L, table_headers_row);
            lua_setfield(L, -2, "TableHeadersRow");
            lua_pushcfunction(L, table_setup_scroll_freeze);
            lua_setfield(L, -2, "TableSetupScrollFreeze");

            lua_setglobal(L, "ImGui");
        }
    }
}
