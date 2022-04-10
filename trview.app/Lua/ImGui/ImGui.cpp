#include "ImGui.h"
#include "../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            /// <summary>
            /// Begin rendering a window.
            /// Parameters:
            ///     table
            ///         string  name
            ///         bool    open
            ///         number  options
            /// </summary>
            int imgui_begin(lua_State* L)
            {
                if (!lua_istable(L, 1))
                {
                    return 0;
                }

                lua_getfield(L, 1, "name");
                const char* name = lua_tostring(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, 1, "options");
                int options = static_cast<ImGuiWindowFlags>(lua_tointeger(L, -1));
                lua_pop(L, 1);

                bool is_open = true;
                bool result = ImGui::Begin(name, &is_open, options);
                lua_pushboolean(L, result);

                lua::set_boolean(L, "open", is_open, 1);

                return 1;
            }

            int imgui_button(lua_State* L)
            {
                bool result = ImGui::Button(lua_tostring(L, 1));
                lua_pushboolean(L, result);
                return 1;
            }

            int imgui_checkbox(lua_State* L)
            {
                if (!lua_istable(L, 1))
                {
                    return 0;
                }

                lua_getfield(L, 1, "name");
                const char* name = lua_tostring(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, 1, "checked");
                bool checked = lua_toboolean(L, -1);
                lua_pop(L, 1);

                bool result = ImGui::Checkbox(name, &checked);
                lua_pushboolean(L, result);
                lua::set_boolean(L, "checked", checked, 1);

                return 1;
            };

            int imgui_text(lua_State* L)
            {
                ImGui::Text(lua_tostring(L, 1));
                return 0;
            }

            int imgui_end(lua_State* L)
            {
                ImGui::End();
                return 0;
            }

            void register_window_flags(lua_State* L)
            {
                lua_newtable(L);
                lua::set_integer(L, "None", ImGuiWindowFlags_None);
                lua::set_integer(L, "NoTitleBar", ImGuiWindowFlags_NoTitleBar);
                lua::set_integer(L, "NoResize", ImGuiWindowFlags_NoResize);
                lua::set_integer(L, "NoMove", ImGuiWindowFlags_NoMove);
                lua::set_integer(L, "NoScrollbar", ImGuiWindowFlags_NoScrollbar);
                lua::set_integer(L, "NoScrollWithMouse", ImGuiWindowFlags_NoScrollWithMouse);
                lua::set_integer(L, "NoCollapse", ImGuiWindowFlags_NoCollapse);
                lua::set_integer(L, "AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize);
                lua::set_integer(L, "NoBackground", ImGuiWindowFlags_NoBackground);
                lua::set_integer(L, "NoSavedSettings", ImGuiWindowFlags_NoSavedSettings);
                lua::set_integer(L, "NoMouseInputs", ImGuiWindowFlags_NoMouseInputs);
                lua::set_integer(L, "MenuBar", ImGuiWindowFlags_MenuBar);
                lua::set_integer(L, "HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar);
                lua::set_integer(L, "NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing);
                lua::set_integer(L, "NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus);
                lua::set_integer(L, "AlwaysVerticalScrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar);
                lua::set_integer(L, "AlwaysHoriztonalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar);
                lua::set_integer(L, "AlwaysUseWindowPadding", ImGuiWindowFlags_AlwaysUseWindowPadding);
                lua::set_integer(L, "NoNavInputs", ImGuiWindowFlags_NoNavInputs);
                lua::set_integer(L, "NoNavFocus", ImGuiWindowFlags_NoNavFocus);
                lua::set_integer(L, "UnsavedDocument", ImGuiWindowFlags_UnsavedDocument);
                lua::set_integer(L, "NoDocking", ImGuiWindowFlags_NoDocking);
                lua::set_integer(L, "NoNav", ImGuiWindowFlags_NoNav);
                lua::set_integer(L, "NoDecoration", ImGuiWindowFlags_NoDecoration);
                lua::set_integer(L, "NoInputs", ImGuiWindowFlags_NoInputs);
                lua_setfield(L, -2, "window_flags");
            }
        }

        void imgui_register(lua_State* L)
        {
            constexpr struct luaL_Reg imgui_lib[] =
            {
                { "begin", imgui_begin },
                { "finish", imgui_end },
                { "text", imgui_text },
                { "button", imgui_button },
                { "checkbox", imgui_checkbox },
                { NULL, NULL },
            };

            luaL_newlib(L, imgui_lib);
            register_window_flags(L);
            lua_setglobal(L, "ImGui");
        }
    }
}
