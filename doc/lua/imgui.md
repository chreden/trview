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

## ImGuiButtonFlags
