local pid = 0
local lara_pointer = 0
local room_offset = 0
local pos_offset = 0
local enabled = false

local selected_game = "Tomb Raider 1"
local games = 
{
    ["Tomb Raider 1"] = { lara = 0, exe = "Tomb1.exe" }, 
    ["Tomb Raider 1 Remastered"] = { lara = 140717555900000, room = 12, position = 20, exe = "tomb123.exe" }, 
    ["Tomb Raider 2"] = { lara = 5375932, room = 24, position = 52, exe = "Tomb2.exe" },
    ["Tomb Raider 2 Remastered"] = { lara = 0, exe = "tomb123.exe" }, 
    ["Tomb Raider 3"] = { lara = 4300048, room = 24, position = 48, exe = "tomb3.exe" },
    ["Tomb Raider 3 Remastered"] = { lara = 0, exe = "tomb123.exe" }, 
    ["Tomb Raider 4"] = { lara = 8446268, room = 24, position = 64, exe = "Tomb4.exe" },
    ["Tomb Raider 5"] = { lara = 0, exe = "Tomb5.exe" }
}

function select_game(game)
    selected_game = game

    local actual_game = games[game]
    if actual_game ~= nil then
        pid = Process.find(actual_game.exe) or 0
        lara_pointer = actual_game.lara
        room_offset = actual_game.room
        pos_offset = actual_game.position
    end
end

function render_ui()
    if (ImGui.Begin( { name = "Lara Tracker" })) then
        if (ImGui.BeginCombo({ label = "A", preview_value = selected_game })) then
            for g, _ in pairs(games) do
                if (ImGui.Selectable({ label = g, selected = g == selected_game })) then
                    select_game(g)
                end
            end
            ImGui.EndCombo()
        end
        ImGui.SameLine()
        if (ImGui.Button({ label = "Reattach" })) then
            select_game(selected_game)
        end
    
        _, pid = ImGui.InputInt( { label = "PID", value = pid })
        _, lara_pointer = ImGui.InputInt( { label = "Lara Pointer", value = lara_pointer })
        _, room_offset = ImGui.InputInt( { label = "Room Offset", value = room_offset })
        _, pos_offset = ImGui.InputInt( { label = "Pos Offset", value = pos_offset })
        _, enabled = ImGui.Checkbox({ label = "Enabled", checked = enabled })

        if (enabled and trview.level ~= nil) then
            local lara_ptr = Process.read_int32(pid, lara_pointer)

            local room = Process.read_int16(pid, lara_ptr + room_offset)
            ImGui.Text( { text = "Room : " .. tostring(room) })
        
            local x = Process.read_int32(pid, lara_ptr + pos_offset)
            local y = Process.read_int32(pid, lara_ptr + pos_offset + 4)
            local z = Process.read_int32(pid, lara_ptr + pos_offset + 8)
            ImGui.Text( { text = "Pos : " .. tostring(x) .. "," .. tostring(y) .. "," .. tostring(z) })
            
            local wp = trview.route.waypoints[1]
            if wp == nil then
                wp = Waypoint.new({position=Vector3.new(1024, 2048, 3072), room=trview.level.rooms[room + 1]})
                trview.route:add(wp)
            end
        
            wp.position = Vector3.new(x, y, z)
            trview.level.selected_room = trview.level.rooms[room + 1]
            trview.camera.target = wp.position
            
        end
            
        ImGui.End()
    end
end

function render_toolbar()
end

print("Lara Tracker loaded")
