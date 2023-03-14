# ImGui

The ImGui library allows plugins to render UI elements.

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| Begin | `boolean result, boolean is_open`  | `{ string name, boolean open, ImGuiWindowFlags flags }` | Begin a new window. `open` and `flags` are optional parameters. `result` indicates whether to continue rendering the window. `is_open` indicates whether the user has closed the window. You must always call `End` even if `Begin` returns `false`. |
| BeginChild |`boolean result`|||
| BeginCombo |`boolean result`|||
| BeginTable |`boolean result`|||
| Button |`boolean result`|||
| Checkbox | `boolean result, boolean checked` | `{ string label, boolean checked }` | Make a checkbox. The checked parameter indicates whether the checkbox is checked and the new state is in the second return value. |
| End ||| End rendering a window |
| EndChild ||||
| EndCombo ||||
| EndTable ||||
| SameLine ||||
| Selectable |`boolean result`|||
| TableHeadersRow ||||
| TableNextColumn |`boolean result`|||
| TableNextRow ||||
| TableSetupColumn ||||
| TableSetupScrollFreeze ||||
| Text || `{ string text } `| Draw some text. |

# Enumerations

## ImGuiWindowFlags

## ImGuiComboFlags

## ImGuiSelectableFlags

## ImGuiTableColumnFlags

## ImGuiTableFlags

## ImGuiButtonFlags
