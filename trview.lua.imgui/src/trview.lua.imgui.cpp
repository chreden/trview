// trview.lua.imgui.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "../inc/trview.lua.imgui.h"

#include <optional>
#include <string>
#include <vector>

#include "trview.lua.imgui.input.h"
#include "tables.h"

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

            int begin(lua_State* L)
            {
                const auto name = get_string(L, 1, "name");
                const auto open = get_optional_bool(L, 1, "open");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool is_open = open.value_or(true);
                bool result = ImGui::Begin(
                    name.c_str(),
                    open.has_value() ? &is_open : nullptr,
                    flags.value_or(ImGuiWindowFlags_None));
                lua_pushboolean(L, result);
                lua_pushboolean(L, is_open);
                return 2;
            }

            int begin_child(lua_State* L)
            {
                const auto name = get_string(L, 1, "name");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::BeginChild(name.c_str(), ImVec2(0,0), false, flags.value_or(ImGuiWindowFlags_None));
                lua_pushboolean(L, result);
                return 1;
            }

            int end(lua_State* L)
            {
                ImGui::End();
                return 0;
            }

            int end_child(lua_State* L)
            {
                ImGui::EndChild();
                return 0;
            }

            int button(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::ButtonEx(label.c_str(), ImVec2(0, 0), flags.value_or(ImGuiButtonFlags_None));
                lua_pushboolean(L, result);
                return 1;
            }

            int checkbox(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                auto checked = get_bool(L, 1, "checked");
                bool result = ImGui::Checkbox(label.c_str(), &checked);

                lua_pushboolean(L, result);
                lua_pushboolean(L, checked);
                return 2;
            }

            int text(lua_State* L)
            {
                auto text = get_string(L, 1, "text");
                ImGui::Text(text.c_str());
                return 0;
            }

            int text_colored(lua_State* L)
            {
                auto text = get_string(L, 1, "text");
                auto r = get_optional_float(L, 1, "r");
                auto g = get_optional_float(L, 1, "g");
                auto b = get_optional_float(L, 1, "b");
                auto a = get_optional_float(L, 1, "a");
                ImGui::TextColored(ImVec4(r.value_or(0), g.value_or(0), b.value_or(0), a.value_or(1)), text.c_str());
                return 0;
            }

            int begin_table(lua_State* L)
            {
                const auto id = get_string(L, 1, "id");
                const auto column = get_integer(L, 1, "column");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::BeginTable(id.c_str(), column, flags.value_or(ImGuiTableFlags_None));
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
                const auto flags = get_optional_integer(L, 1, "flags");
                ImGui::TableSetupColumn(label.c_str(), flags.value_or(ImGuiTableColumnFlags_None));
                return 0;
            }

            int table_next_column(lua_State* L)
            {
                lua_pushboolean(L, ImGui::TableNextColumn());
                return 1;
            }

            int table_next_row(lua_State* L)
            {
                ImGui::TableNextRow();
                return 0;
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

            int selectable(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                auto selected = get_bool(L, 1, "selected");
                auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::Selectable(label.c_str(), &selected, flags.value_or(ImGuiSelectableFlags_None));
                lua_pushboolean(L, result);
                return 1;
            }

            int begin_combo(lua_State* L)
            {
                const auto label = get_string(L, 1, "label");
                const auto preview_value = get_string(L, 1, "preview_value");
                const auto flags = get_optional_integer(L, 1, "flags");
                bool result = ImGui::BeginCombo(label.c_str(), preview_value.c_str(), flags.value_or(ImGuiComboFlags_None));
                lua_pushboolean(L, result);
                return 1;
            }

            int end_combo(lua_State* L)
            {
                ImGui::EndCombo();
                return 0;
            }

            int same_line(lua_State* L)
            {
                ImGui::SameLine();
                return 0;
            }
        }

        void imgui_register(lua_State* L)
        {
            lua_newtable(L);
            // Windows
            lua_pushcfunction(L, begin);
            lua_setfield(L, -2, "Begin");
            lua_pushcfunction(L, begin_child);
            lua_setfield(L, -2, "BeginChild");
            lua_pushcfunction(L, end);
            lua_setfield(L, -2, "End");
            lua_pushcfunction(L, end_child);
            lua_setfield(L, -2, "EndChild");

            set_enum(L, "WindowFlags", -1,
                {
                    { "None", ImGuiWindowFlags_None },
                    { "NoTitleBar", ImGuiWindowFlags_NoTitleBar },
                    { "NoResize", ImGuiWindowFlags_NoResize },
                    { "NoMove", ImGuiWindowFlags_NoMove },
                    { "NoScrollbar", ImGuiWindowFlags_NoScrollbar },
                    { "NoScrollWithMouse", ImGuiWindowFlags_NoScrollWithMouse },
                    { "NoCollapse", ImGuiWindowFlags_NoCollapse },
                    { "AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize },
                    { "NoBackground", ImGuiWindowFlags_NoBackground },
                    { "NoSavedSettings", ImGuiWindowFlags_NoSavedSettings },
                    { "NoMouseInputs", ImGuiWindowFlags_NoMouseInputs },
                    { "MenuBar", ImGuiWindowFlags_MenuBar },
                    { "HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar },
                    { "NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing },
                    { "NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus },
                    { "AlwaysVerticalScrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar },
                    { "AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar },
                    { "AlwaysUseWindowPadding", ImGuiWindowFlags_AlwaysUseWindowPadding },
                    { "NoNavInputs", ImGuiWindowFlags_NoNavInputs },
                    { "NoNavFocus", ImGuiWindowFlags_NoNavFocus },
                    { "UnsavedDocument", ImGuiWindowFlags_UnsavedDocument },
                    { "NoDocking", ImGuiWindowFlags_NoDocking }
                });

            // Buttons
            lua_pushcfunction(L, button);
            lua_setfield(L, -2, "Button");

            set_enum(L, "ButtonFlags", -1,
                {
                    { "None", ImGuiButtonFlags_None },
                    { "MouseButtonLeft", ImGuiButtonFlags_MouseButtonLeft },
                    { "MouseButtonRight", ImGuiButtonFlags_MouseButtonRight },
                    { "MouseButtonMiddle", ImGuiButtonFlags_MouseButtonMiddle },
                    { "PressedOnClick", ImGuiButtonFlags_PressedOnClick },
                    { "PressedOnClickRelease", ImGuiButtonFlags_PressedOnClickRelease },
                    { "PressedOnClickReleaseAnywhere", ImGuiButtonFlags_PressedOnClickReleaseAnywhere },
                    { "PressedOnRelease", ImGuiButtonFlags_PressedOnRelease },
                    { "PressedOnDoubleClick", ImGuiButtonFlags_PressedOnDoubleClick },
                    { "PressedOnDragDropHold", ImGuiButtonFlags_PressedOnDragDropHold },
                    { "Repeat", ImGuiButtonFlags_Repeat },
                    { "FlattenChildren", ImGuiButtonFlags_FlattenChildren },
                    { "DontClosePopups", ImGuiButtonFlags_DontClosePopups },
                    { "AlignTextBaseLine", ImGuiButtonFlags_AlignTextBaseLine },
                    { "NoKeyModifiers", ImGuiButtonFlags_NoKeyModifiers },
                    { "NoHoldingActiveId", ImGuiButtonFlags_NoHoldingActiveId },
                    { "NoNavFocus", ImGuiButtonFlags_NoNavFocus },
                    { "NoHoveredOnFocus", ImGuiButtonFlags_NoHoveredOnFocus },
                    { "PressedOnMask_", ImGuiButtonFlags_PressedOnMask_ },
                    { "PressedOnDefault_", ImGuiButtonFlags_PressedOnDefault_ },
                });

            // Checkbox
            lua_pushcfunction(L, checkbox);
            lua_setfield(L, -2, "Checkbox");
            // Combo
            lua_pushcfunction(L, begin_combo);
            lua_setfield(L, -2, "BeginCombo");
            lua_pushcfunction(L, end_combo);
            lua_setfield(L, -2, "EndCombo");

            set_enum(L, "ComboFlags", -1,
                {
                    { "None", ImGuiComboFlags_None },
                    { "PopupAlignLeft", ImGuiComboFlags_PopupAlignLeft },
                    { "HeightSmall", ImGuiComboFlags_HeightSmall },
                    { "HeightRegular", ImGuiComboFlags_HeightRegular },
                    { "HeightLarge", ImGuiComboFlags_HeightLarge },
                    { "HeightLargest", ImGuiComboFlags_HeightLargest },
                    { "NoArrowButton", ImGuiComboFlags_NoArrowButton },
                    { "NoPreview", ImGuiComboFlags_NoPreview }
                });

            // Text
            lua_pushcfunction(L, text);
            lua_setfield(L, -2, "Text");
            lua_pushcfunction(L, text_colored);
            lua_setfield(L, -2, "TextColored");
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
            lua_pushcfunction(L, table_next_row);
            lua_setfield(L, -2, "TableNextRow");
            lua_pushcfunction(L, table_setup_scroll_freeze);
            lua_setfield(L, -2, "TableSetupScrollFreeze");

            set_enum(L, "TableFlags", -1,
                {
                    { "None", ImGuiTableFlags_None },
                    { "Resizable", ImGuiTableFlags_Resizable },
                    { "Reorderable", ImGuiTableFlags_Reorderable },
                    { "Hideable", ImGuiTableFlags_Hideable },
                    { "Sortable", ImGuiTableFlags_Sortable },
                    { "NoSavedSettings", ImGuiTableFlags_NoSavedSettings },
                    { "ContextMenuInBody", ImGuiTableFlags_ContextMenuInBody },
                    { "RowBg", ImGuiTableFlags_RowBg },
                    { "BordersInnerH", ImGuiTableFlags_BordersInnerH },
                    { "BordersOuterH", ImGuiTableFlags_BordersOuterH },
                    { "BordersInnerV", ImGuiTableFlags_BordersInnerV },
                    { "BordersOuterV", ImGuiTableFlags_BordersOuterV },
                    { "BordersH", ImGuiTableFlags_BordersH },
                    { "BordersV", ImGuiTableFlags_BordersV },
                    { "BordersInner", ImGuiTableFlags_BordersInner },
                    { "BordersOuter", ImGuiTableFlags_BordersOuter },
                    { "Borders", ImGuiTableFlags_Borders },
                    { "NoBordersInBody", ImGuiTableFlags_NoBordersInBody },
                    { "NoBordersInBodyUntilResize", ImGuiTableFlags_NoBordersInBodyUntilResize },
                    { "SizingFixedFit", ImGuiTableFlags_SizingFixedFit },
                    { "SizingFixedSame", ImGuiTableFlags_SizingFixedSame },
                    { "SizingStretchProp", ImGuiTableFlags_SizingStretchProp },
                    { "SizingStretchSame", ImGuiTableFlags_SizingStretchSame },
                    { "NoHostExtendX", ImGuiTableFlags_NoHostExtendX },
                    { "NoHostExtendY", ImGuiTableFlags_NoHostExtendY },
                    { "NoKeepColumnsVisible", ImGuiTableFlags_NoKeepColumnsVisible },
                    { "PreciseWidths", ImGuiTableFlags_PreciseWidths },
                    { "NoClip", ImGuiTableFlags_NoClip },
                    { "PadOuterX", ImGuiTableFlags_PadOuterX },
                    { "NoPadOuterX", ImGuiTableFlags_NoPadOuterX },
                    { "NoPadInnerX", ImGuiTableFlags_NoPadInnerX },
                    { "ScrollX", ImGuiTableFlags_ScrollX },
                    { "ScrollY", ImGuiTableFlags_ScrollY },
                    { "SortMulti", ImGuiTableFlags_SortMulti },
                    { "SortTristate", ImGuiTableFlags_SortTristate }
                });

            set_enum(L, "TableColumnFlags", -1,
                {
                    { "None", ImGuiTableColumnFlags_None },
                    { "Disabled", ImGuiTableColumnFlags_Disabled },
                    { "DefaultHide", ImGuiTableColumnFlags_DefaultHide },
                    { "DefaultSort", ImGuiTableColumnFlags_DefaultSort },
                    { "WidthStretch", ImGuiTableColumnFlags_WidthStretch },
                    { "WidthFixed", ImGuiTableColumnFlags_WidthFixed },
                    { "NoResize", ImGuiTableColumnFlags_NoResize },
                    { "NoReorder", ImGuiTableColumnFlags_NoReorder },
                    { "NoHide", ImGuiTableColumnFlags_NoHide },
                    { "NoClip", ImGuiTableColumnFlags_NoClip },
                    { "NoSort", ImGuiTableColumnFlags_NoSort },
                    { "NoSortAscending", ImGuiTableColumnFlags_NoSortAscending },
                    { "NoSortDescending", ImGuiTableColumnFlags_NoSortDescending },
                    { "NoHeaderLabel", ImGuiTableColumnFlags_NoHeaderLabel },
                    { "NoHeaderWidth", ImGuiTableColumnFlags_NoHeaderWidth },
                    { "PreferSortAscending", ImGuiTableColumnFlags_PreferSortAscending },
                    { "PreferSortDescending", ImGuiTableColumnFlags_PreferSortDescending },
                    { "IndentEnable", ImGuiTableColumnFlags_IndentEnable },
                    { "IndentDisable", ImGuiTableColumnFlags_IndentDisable },
                    { "IsEnabled", ImGuiTableColumnFlags_IsEnabled },
                    { "IsVisible", ImGuiTableColumnFlags_IsVisible },
                    { "IsSorted", ImGuiTableColumnFlags_IsSorted },
                    { "IsHovered", ImGuiTableColumnFlags_IsHovered }
                });

            // Selectable
            lua_pushcfunction(L, selectable);
            lua_setfield(L, -2, "Selectable");

            set_enum(L, "SelectableFlags", -1, 
                {
                    { "None", ImGuiSelectableFlags_None },
                    { "DontClosePopups", ImGuiSelectableFlags_DontClosePopups },
                    { "SpanAllColumns", ImGuiSelectableFlags_SpanAllColumns },
                    { "AllowDoubleClick", ImGuiSelectableFlags_AllowDoubleClick },
                    { "Disabled", ImGuiSelectableFlags_Disabled },
                    { "AllowItemOverlap", ImGuiSelectableFlags_AllowItemOverlap },
                    { "NoHoldingActiveID", ImGuiSelectableFlags_NoHoldingActiveID },
                    { "SelectOnNav", ImGuiSelectableFlags_SelectOnNav },
                    { "SelectOnClick", ImGuiSelectableFlags_SelectOnClick },
                    { "SelectOnRelease", ImGuiSelectableFlags_SelectOnRelease },
                    { "SpanAvailWidth", ImGuiSelectableFlags_SpanAvailWidth },
                    { "SetNavIdOnHover", ImGuiSelectableFlags_SetNavIdOnHover },
                    { "NoPadWithHalfSpacing", ImGuiSelectableFlags_NoPadWithHalfSpacing }
                });

            lua_pushcfunction(L, same_line);
            lua_setfield(L, -2, "SameLine");

            register_input(L);

            lua_setglobal(L, "ImGui");
        }
    }
}
