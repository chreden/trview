// trview.lua.imgui.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "trview.lua.imgui.h"

#include <external/imgui/imgui.h>

namespace trview
{
    namespace lua
    {
        namespace
        {
            int begin(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                lua_getfield(L, 1, "name");
                luaL_checktype(L, -1, LUA_TSTRING);
                bool result = ImGui::Begin(lua_tostring(L, -1), 0, 0);
                lua_pop(L, 1);
                lua_pushboolean(L, result);
                return 1;
            }

            int finish(lua_State* L)
            {
                ImGui::End();
                return 0;
            }

            int button(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TSTRING);
                bool result = ImGui::Button(lua_tostring(L, 1), ImVec2(0, 0));
                lua_pushboolean(L, result);
                return 1;
            }

            int checkbox(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                lua_getfield(L, 1, "label");
                luaL_checktype(L, -1, LUA_TSTRING);
                lua_getfield(L, 1, "checked");
                luaL_checktype(L, -1, LUA_TBOOLEAN);

                bool checked = lua_toboolean(L, -1);
                bool result = ImGui::Checkbox(lua_tostring(L, -2), &checked);
                
                lua_pushboolean(L, result);
                lua_pushboolean(L, checked);
                return 2;
            }

            int text(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TTABLE);
                lua_getfield(L, 1, "text");
                luaL_checktype(L, -1, LUA_TSTRING);
                ImGui::Text(lua_tostring(L, -1));
                return 0;
            }
        }

        void imgui_register(lua_State* L)
        {
            lua_newtable(L);
            lua_pushcfunction(L, begin);
            lua_setfield(L, -2, "Begin");
            lua_pushcfunction(L, finish);
            lua_setfield(L, -2, "End");
            lua_pushcfunction(L, button);
            lua_setfield(L, -2, "Button");
            lua_pushcfunction(L, checkbox);
            lua_setfield(L, -2, "Checkbox");
            lua_pushcfunction(L, text);
            lua_setfield(L, -2, "Text");
            lua_setglobal(L, "ImGui");
        }
    }
}
