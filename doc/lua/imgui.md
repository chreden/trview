# ImGui

The ImGui library allows plugins to render UI elements.

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| Begin | `boolean result, boolean is_open`  | `{ string name, boolean open, ImGuiWindowFlags flags }` | Begin a new window. `open` and `flags` are optional parameters. `result` indicates whether to continue rendering the window. `is_open` indicates whether the user has closed the window. You must always call `End` even if `Begin` returns `false`. |
| BeginChild |`boolean result`|`{ string name, ImGuiWindowFlags flags }`||
| BeginCombo |`boolean result`|||
| BeginTable |`boolean result`|`{ string id, number column, ImGuiTableFlags flags }`||
| Button |`boolean result`|`{ string label, ImGuiButtonFlags flags }`||
| Checkbox | `boolean result, boolean checked` | `{ string label, boolean checked }` | Make a checkbox. The checked parameter indicates whether the checkbox is checked and the new state is in the second return value. |
| End ||| End rendering a window |
| EndChild ||||
| EndCombo ||||
| EndTable ||||
| InputDouble | `boolean result, number value` |||
| InputFloat | `boolean result, number value` |||
| InputFloat2 | `boolean result, number value1, number value2` |||
| InputFloat3 | `boolean result, number value1, number value2, number value3` |||
| InputFloat4 | `boolean result, number value1, number value2, number value3, number value4` |||
| InputInt | `boolean result, number value` |||
| InputInt2 | `boolean result, number value1, number value2` |||
| InputInt3 | `boolean result, number value1, number value2, number value3` |||
| InputInt4 | `boolean result, number value1, number value2, number value3, number value4` |||
| InputText | `boolean result, string value` |||
| SameLine ||||
| Selectable |`boolean result`|`{ string label, boolean selected, ImGuiSelectableFlags flags }`||
| TableHeadersRow ||||
| TableNextColumn |`boolean result`|||
| TableNextRow ||||
| TableSetupColumn ||`{ string label, ImGuiTableColumnFlags flags }`||
| TableSetupScrollFreeze ||`{ number cols, number rows }`||
| Text || `{ string text } `| Draw some text. |

# Enumerations

## ImGuiWindowFlags

## ImGuiComboFlags

## ImGuiSelectableFlags

## ImGuiTableColumnFlags

## ImGuiTableFlags

## ImGuiTextFlags

| Name |
| ---- |
| None |
| CharsDecimal |
| CharsHexadecimal |
| CharsUppercase |
| CharsNoBlank |
| AutoSelectAll |
| EnterReturnsTrue |
| AllowTabInput |
| CtrlEnterForNewLine |
| NoHorizontalScroll |
| AlwaysOverwrite |
| ReadOnly |
| Password |
| NoUndoRedo |
| CharsScientific |

## ImGuiButtonFlags
