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
| EndChild ||| End a child window |
| EndCombo ||||
| EndTable ||||
| InputDouble | `boolean result, number value` | `{ string label, number value, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputFloat | `boolean result, number value` | `{ string label, number value, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputFloat2 | `boolean result, number value1, number value2` | `{ string label, number value1, number value2, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputFloat3 | `boolean result, number value1, number value2, number value3` | `{ string label, number value1, number value2, number value3, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputFloat4 | `boolean result, number value1, number value2, number value3, number value4` | `{ string label, number value1, number value2, number value3, number value4, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputInt | `boolean result, number value` | `{ string label, number value, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputInt2 | `boolean result, number value1, number value2` | `{ string label, number value1, number value2 [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputInt3 | `boolean result, number value1, number value2, number value3` | `{ string label, number value1, number value2, number value3, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputInt4 | `boolean result, number value1, number value2, number value3, number value4` | `{ string label, number value1, number value2, number value3, number value4, [optional] number step, [optional] number step_fast, [optional] string format, [optional] InputTextFlags flags }` ||
| InputText | `boolean result, string value` | `{ string label, string value, [optional] InputTextFlags flags }` ||
| SameLine ||| Put the next element on the same line. |
| Selectable |`boolean result`|`{ string label, boolean selected, ImGuiSelectableFlags flags }`||
| TableHeadersRow ||||
| TableNextColumn |`boolean result`|||
| TableNextRow ||||
| TableSetupColumn ||`{ string label, ImGuiTableColumnFlags flags }`||
| TableSetupScrollFreeze ||`{ number cols, number rows }`||
| Text || `{ string text } `| Draw some text. |

# Enumerations

## WindowFlags

| Name | 
| ---- |
| None |
| NoTitleBar |
| NoResize |
| NoMove |
| NoScrollbar |
| NoScrollWithMouse |
| NoCollapse |
| AlwaysAutoResize |
| NoBackground |
| NoSavedSettings |
| NoMouseInputs |
| MenuBar |
| HorizontalScrollbar |
| NoFocusOnAppearing |
| NoBringToFrontOnFocus |
| AlwaysVerticalScrollbar |
| AlwaysHorizontalScrollbar |
| AlwaysUseWindowPadding |
| NoNavInputs |
| NoNavFocus |
| UnsavedDocument |
| NoDocking |


## ComboFlags

| Name |
| ---- |
| None |
| PopupAlignLeft |
| HeightSmall |
| HeightRegular |
| HeightLarge |
| HeightLargest |
| NoArrowButton |
| NoPreview |

## SelectableFlags

| Name |
| ---- |
| None |
| DontClosePopups |
| SpanAllColumns |
| AllowDoubleClick |
| Disabled |
| AllowItemOverlap |
| NoHoldingActiveID |
| SelectOnNav |
| SelectOnClick |
| SelectOnRelease |
| SpanAvailWidth |
| DrawHoveredWhenHeld |
| SetNavIdOnHover |
| NoPadWithHalfSpacing |

## TableColumnFlags

| Name |
| ---- |
| None |
| Disabled |
| DefaultHide |
| DefaultSort |
| WidthStretch |
| WidthFixed |
| NoResize |
| NoReorder |
| NoHide |
| NoClip |
| NoSort |
| NoSortAscending |
| NoSortDescending |
| NoHeaderLabel |
| NoHeaderWidth |
| PreferSortAscending |
| PreferSortDescending |
| IndentEnable |
| IndentDisable |
| IsEnabled |
| IsVisible |
| IsSorted |
| IsHovered |

## TableFlags

| Name | Notes |
| ---- | ----- |
| None ||
| Resizable ||
| Reorderable ||
| Hideable ||
| Sortable | Not yet implemented |
| NoSavedSettings ||
| ContextMenuInBody ||
| RowBg ||
| BordersInnerH ||
| BordersOuterH ||
| BordersInnerV ||
| BordersOuterV ||
| BordersH ||
| BordersV ||
| BordersInner ||
| BordersOuter ||
| Borders ||
| NoBordersInBody ||
| NoBordersInBodyUntilResize ||
| SizingFixedFit ||
| SizingFixedSame ||
| SizingStretchProp ||
| SizingStretchSame ||
| NoHostExtendX ||
| NoHostExtendY ||
| NoKeepColumnsVisible ||
| PreciseWidths ||
| NoClip ||
| PadOuterX ||
| NoPadOuterX ||
| NoPadInnerX ||
| ScrollX ||
| ScrollY ||
| SortMulti | Not yet implemented |
| SortTristate | Not yet implemented |

## InputTextFlags

| Name | Notes |
| ---- | ----- |
| None ||
| CharsDecimal ||
| CharsHexadecimal ||
| CharsUppercase ||
| CharsNoBlank ||
| AutoSelectAll ||
| EnterReturnsTrue ||
| CallbackCompletion | Not yet implemented |
| CallbackHistory | Not yet implemented |
| CallbackAlways | Not yet implemented |
| CallbackCharFilter | Not yet implemented |
| AllowTabInput ||
| CtrlEnterForNewLine ||
| NoHorizontalScroll ||
| AlwaysOverwrite ||
| ReadOnly ||
| Password ||
| NoUndoRedo ||
| CharsScientific ||
| CallbackResize | Not yet implemented |
| CallbackEdit | Not yet implemented |

## ButtonFlags

| Name |
| ---- |
| None |
| MouseButtonLeft |
| MouseButtonRight |
| MouseButtonMiddle |
| PressedOnClick |
| PressedOnClickRelease |
| PressedOnClickReleaseAnywhere |
| PressedOnRelease |
| PressedOnDoubleClick |
| PressedOnDragDropHold |
| Repeat |
| FlattenChildren |
| AllowItemOverlap |
| DontClosePopups |
| AlignTextBaseLine |
| NoKeyModifiers |
| NoHoldingActiveId |
| NoNavFocus |
| NoHoveredOnFocus |
| PressedOnMask_ |
| PressedOnDefault_ |
