local pid = 0
local base_pointer = 140696139255372
local game = nil

local selected_game = "Tomb Raider 1 Remastered"
local games = 
{
    ["Tomb Raider 1 Remastered"] = { exe = "tomb123.exe", entity_size = 3664, room_offset = 12, x_offset = 72, y_offset = 76, z_offset = 80 },
    ["Tomb Raider 2 Remastered"] = { exe = "tomb123.exe", entity_size = 3664, room_offset = 12, x_offset = 72, y_offset = 76, z_offset = 80 },
    ["Tomb Raider 3 Remastered"] = { exe = "tomb123.exe", entity_size = 3664, room_offset = 12, x_offset = 72, y_offset = 76, z_offset = 80 },
    ["Tomb Raider 4 Remastered"] = { exe = "tomb456.exe", entity_size = 9416, room_offset = 12, x_offset = 80, y_offset = 84, z_offset = 88 },
    ["Tomb Raider 5 Remastered"] = { exe = "tomb456.exe", entity_size = 9416, room_offset = 12, x_offset = 80, y_offset = 84, z_offset = 88 },
}

function select_game(new_game)
    selected_game = new_game

    local actual_game = games[new_game]
    if actual_game ~= nil then
        pid = Process.find(actual_game.exe) or 0
        game = actual_game
    end
end

function render_ui()
    if (ImGui.Begin( { name = "NG+ Scanner" })) then
    
        if (ImGui.BeginCombo({ label = "Game", preview_value = selected_game })) then
            for g, _ in pairs(games) do
                if (ImGui.Selectable({ label = g, selected = g == selected_game })) then
                    select_game(g)
                end
            end
            ImGui.EndCombo()
        end
        
        local base_text = nil
        _, base_text = ImGui.InputText({ label = "Base", value = tostring(base_pointer) })
        base_pointer = tonumber(base_text)

        ImGui.SameLine()
        if (ImGui.Button({ label = "Reattach" })) then
            select_game(selected_game)
        end

        if (pid ~= 0) then
            if (trview.level ~= nil) then
                local new_items = ""
                local changed_items = ""

                local ptr = base_pointer
                for i = 0, 256 do
                    local entity_ptr = ptr + i * game.entity_size
                    local entity_type = Process.read_int16(pid, entity_ptr)
                    if (entity_type == 0) then
                        if (Process.read_int16(pid, entity_ptr + 2) == 0) then
                            break
                        end
                    end

                    local all_items = {}
                    for _, item in pairs(trview.level.items) do
                        if not item.ai then table.insert(all_items, item) end
                    end

                    if i >= #all_items then
                        if new_items ~= "" then
                            new_items = new_items .. ",\n"
                        end

                        local new_type_id = math.tointeger(entity_type)
                        local new_room = math.tointeger(Process.read_int16(pid, entity_ptr + game.room_offset))
                        local new_x = math.tointeger(Process.read_int32(pid, entity_ptr + game.x_offset))
                        local new_y = math.tointeger(Process.read_int32(pid, entity_ptr + game.y_offset))
                        local new_z = math.tointeger(Process.read_int32(pid, entity_ptr + game.z_offset))
                        new_items = new_items .. "{\"NgPlus\":true,\"Index\":" .. i .. ",\"TypeID\":" .. new_type_id .. ",\"Room\":" .. new_room ..",\"X\":" .. new_x .. ",\"Y\":" .. new_y .. ",\"Z\":" .. new_z .. "}"

                    elseif all_items[i + 1].type_id ~= entity_type then
                        if changed_items ~= "" then
                            changed_items = changed_items .. ",\n"
                        end
                        changed_items = changed_items .. "                { \"item\":" .. i .. ", \"type\":" .. math.tointeger(entity_type) .. " }"
                    end
                end
                
                ImGui.InputTextMultiline({ label = "New", value = new_items })
                ImGui.InputTextMultiline({ label = "Changed", value = changed_items })
            end
        end
        
        ImGui.End()
    end
end

function render_toolbar()
end

print("NG+ Scanner loaded")
