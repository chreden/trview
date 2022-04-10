function find_monkey()
    if trview.level == nil then return end

    local monkeys = {}
    for _, e in pairs(trview.level.items) do
        if (e.type == "Monkey") then
            table.insert(monkeys, e)
        end
    end
    
    local index = math.random(#monkeys)
    trview.select_item(monkeys[index])
end

function render_ui()
    if (ImGui.begin { name = "Monkey Finder" }) then
        if (ImGui.button("Find a monkey!")) then
            find_monkey()
        end
        ImGui.finish()
    end
end
