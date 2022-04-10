--[[

    Slope Directions plugin

    Detects the directions of the slopes in the world and overlays them in the 3D view.

    Has option to toggle visibility of the overlay.

--]]

local window = { name = "Slope Direction", open = true, options = ImGui.window_flags.AlwaysAutoResize }
local options = { visible = false }
local data = { rooms = { } } 

function generate()
    if trview.level == nil then return end

    data.rooms = { }
    for _, room in pairs(trview.level.rooms) do
        local floor_slant_count = 0
        for _, sector in pairs(room.sectors) do
            if (sector.flags & 16 == 16) then floor_slant_count = floor_slant_count + 1 end
        end
        data.rooms[room.number + 1] = floor_slant_count
    end
end

function render()
    -- Render the generated geometry
end

function render_ui()
    if (window.open and ImGui.begin(window)) then

        local show_directions = { name = "Show Directions", checked = options.visible }
        if (ImGui.checkbox(show_directions)) then
            options.visible = show_directions.checked
        end

        if (ImGui.button("Generate")) then
            generate()
        end

        -- Render the table of information.
        ImGui.text("Floor Slants:")
        for _, room in pairs(data.rooms) do
            ImGui.text((_ - 1) .. " - " .. room)
        end

        ImGui.finish()
    end
end
