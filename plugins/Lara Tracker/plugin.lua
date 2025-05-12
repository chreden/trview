local pid = 145812
local offset = 5375932
local enabled = false

-- 1207BC
-- 5207BC

-- 2D62A4
-- 6D62A4

function render_ui()
    
    if (ImGui.Begin( { name = "lmao" })) then
        ImGui.Text( { text = "Ayyyy" })

        _, pid = ImGui.InputInt( { label = "PID", value = pid })
        _, offset = ImGui.InputInt( { label = "Offset", value = offset })
        _, enabled = ImGui.Checkbox({ label = "Enabled", checked = enabled })
        
        if (enabled and trview.level ~= nil) then
            local lara_ptr = trview.read_int(pid, offset)

            local room = trview.read_int16(pid, lara_ptr + 24)
            ImGui.Text( { text = "Room : " .. tostring(room) })
        
            local x = trview.read_int(pid, lara_ptr + 52)
            local y = trview.read_int(pid, lara_ptr + 56)
            local z = trview.read_int(pid, lara_ptr + 60)
            
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
