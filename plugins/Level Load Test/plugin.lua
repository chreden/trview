local show_window = false
local scripts_txt_path = "C:/dev/trview-level-tests/tests.txt"
local levels_directory = "C:/dev/tomblevels/TombLevels/"
local scripts = {}

function load_level_tests()
    print("Reading tests.txt at location: " .. scripts_txt_path)

    local base_path_start, base_path_end = scripts_txt_path:find(".*[/\\]")
    local base_path = scripts_txt_path:sub(base_path_start, base_path_end)
    print("Base path: " .. base_path)

    local script_paths = {}
    for script in io.lines(scripts_txt_path) do
        table.insert(script_paths, base_path .. script)
    end

    for _, v in pairs(script_paths) do print("Script: " .. v) end

    scripts = {}
    for _, path in pairs(script_paths) do
        dofile(path)
        local name, info = get_test_info()
        table.insert(scripts, { name = name, info = info, path = path, pass = nil, in_progress = false, text = "" })
    end
end

function render_ui()
    if not show_window then
        return
    end
    
    for _, script in pairs(scripts) do
        if script.in_progress then
            local pass, text = run_test(levels_directory)
            script.pass = pass
            script.text = text
            script.in_progress = script.pass == nil
        end
    end

    if (ImGui.Begin({ name = "Level Load Test" })) then

        ImGui.TextWrapped( { text = "Enter the path to tests.txt that lists scripts to run and the directory where levels are stored"})

        if (ImGui.Button({label = "Reload Tests"})) then
            load_level_tests()
        end

        local result, new_scripts_txt_path = ImGui.InputText({ label = "Script.txt path", value = scripts_txt_path })
        if result then scripts_txt_path = new_scripts_txt_path end

        local result, new_levels_directory = ImGui.InputText({ label = "Levels path", value = levels_directory })
        if result then levels_directory = new_levels_directory end
        
        if (ImGui.BeginTable( { id = "Tests", column = 5, flags = ImGui.TableFlags.SizingStretchProp })) then
            
            ImGui.TableSetupColumn({ label = "Run", flags = ImGui.TableColumnFlags.WidthStretch })
            ImGui.TableSetupColumn({ label = "Name", flags = ImGui.TableColumnFlags.WidthStretch})
            ImGui.TableSetupColumn({ label = "Info", flags = ImGui.TableColumnFlags.WidthStretch})
            ImGui.TableSetupColumn({ label = "Pass", flags = ImGui.TableColumnFlags.WidthStretch})
            ImGui.TableSetupColumn({ label = "Path", flags = ImGui.TableColumnFlags.WidthStretch})
            ImGui.TableSetupScrollFreeze({ cols = 0, rows = 1 })
            ImGui.TableHeadersRow()

            for _, script in pairs(scripts) do
                ImGui.TableNextRow()
                ImGui.TableNextColumn()
                if (ImGui.Button({ label = "Run" })) then
                   script.in_progress = true
                   dofile(script.path)
                end
                ImGui.TableNextColumn()
                ImGui.Text({ text = script.name })
                ImGui.TableNextColumn()
                ImGui.Text({ text = script.info })
                ImGui.TableNextColumn()
                ImGui.Text({ text = tostring(script.pass)} )
                ImGui.TableNextColumn()
                ImGui.Text({ text = script.text })
            end

            ImGui.EndTable()
        end

        ImGui.End()
    end
end

function render_toolbar()
    if (ImGui.Button({ label = "Level Load Test" })) then
        show_window = not show_window
    end
end

print("Level Load Test loaded")
