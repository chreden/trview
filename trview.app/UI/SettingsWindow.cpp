#include "SettingsWindow.h"
#include <dwrite.h>

namespace trview
{
    namespace
    {
        std::vector<FontSetting> get_ye_fonts()
        {
            std::vector<FontSetting> available_fonts;

            HKEY key = nullptr;
            if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &key))
            {
                wchar_t class_name[255];
                DWORD class_name_length = 255;
                DWORD number_of_subkeys = 0;
                DWORD max_subkey_length = 0;
                DWORD max_class_length = 0;
                DWORD value_count = 0;
                DWORD max_value_length = 0;
                DWORD max_data_length = 0;
                DWORD security_descriptor = 0;
                FILETIME last_write_time;

                std::vector<wchar_t> value(16383, 0);
                DWORD value_length = 16383;

                LRESULT result = RegQueryInfoKey(key,
                    class_name,
                    &class_name_length,
                    nullptr,
                    &number_of_subkeys,
                    &max_subkey_length,
                    &max_class_length,
                    &value_count,
                    &max_value_length,
                    &max_data_length,
                    &security_descriptor,
                    &last_write_time);

                for (uint32_t i = 0; i < value_count; ++i)
                {
                    value_length = 16383;
                    value[0] = '\0';

                    DWORD data_length = 0;
                    result = RegEnumValue(key, i, &value[0], &value_length, NULL, NULL, NULL, &data_length);

                    value_length = 16383;

                    std::vector<uint8_t> data_value(data_length, 0);
                    result = RegEnumValue(key, i, &value[0], &value_length, NULL, NULL, &data_value[0], &data_length);

                    wchar_t* value_string = reinterpret_cast<wchar_t*>(&data_value[0]);
                    if (value_string && std::wstring(value_string).ends_with(L".ttf"))
                    {
                        available_fonts.push_back(
                            {
                                .name = to_utf8(&value[0]),
                                .filename = to_utf8(value_string)
                            });
                    }
                }
            }

            RegCloseKey(key);

            return available_fonts;
        }
    }

    ISettingsWindow::~ISettingsWindow()
    {
    }

    SettingsWindow::SettingsWindow(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IShell>& shell)
        : _dialogs(dialogs), _shell(shell)
    {
    }

    void SettingsWindow::render()
    {
        if (!_visible)
        {
            return;
        }

        const auto checkbox = [](const std::string& name, bool& setting, Event<bool>& event)
        {
            if (ImGui::Checkbox(name.c_str(), &setting))
            {
                event(setting);
            }
        };

        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Settings", &_visible, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("General"))
                {
                    checkbox(Names::vsync, _vsync, on_vsync);
                    checkbox(Names::go_to_lara, _go_to_lara, on_go_to_lara);
                    checkbox(Names::items_startup, _items_startup, on_items_startup);
                    checkbox(Names::triggers_startup, _triggers_startup, on_triggers_startup);
                    checkbox(Names::rooms_startup, _rooms_startup, on_rooms_startup);
                    checkbox(Names::route_startup, _route_startup, on_route_startup);
                    checkbox(Names::camera_sink_startup, _camera_sink_startup, on_camera_sink_startup);
                    checkbox(Names::randomizer_tools, _randomizer_tools, on_randomizer_tools);
                    if (ImGui::InputInt(Names::max_recent_files.c_str(), &_max_recent_files))
                    {
                        _max_recent_files = std::max(0, _max_recent_files);
                        on_max_recent_files(_max_recent_files);
                    }
                    if (ImGui::ColorEdit3(Names::background_colour.c_str(), _colour))
                    {
                        on_background_colour(Colour(1.0f, _colour[0], _colour[1], _colour[2]));
                    }

                    if (ImGui::BeginCombo("Font", _selected_font.name.c_str()))
                    {
                        for (const auto& f : get_ye_fonts())
                        {
                            bool is_selected = _selected_font.name == f.name;
                            if (ImGui::Selectable(f.name.c_str(), is_selected))
                            {
                                _selected_font.name = f.name;
                                _selected_font.filename = f.filename;
                                on_font(_selected_font);
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::InputFloat("Font Size", &_selected_font.size, 1.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        on_font(_selected_font);
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Camera"))
                {
                    checkbox(Names::invert_vertical_pan, _invert_vertical_panning, on_invert_vertical_pan);
                    checkbox(Names::camera_display_degrees, _camera_display_degrees, on_camera_display_degrees);
                    checkbox(Names::auto_orbit, _auto_orbit, on_auto_orbit);
                    if (ImGui::SliderFloat(Names::sensitivity.c_str(), &_sensitivity, 0.0f, 1.0f))
                    {
                        on_sensitivity_changed(_sensitivity);
                    }
                    if (ImGui::SliderFloat(Names::movement_speed.c_str(), &_movement_speed, 0.0f, 1.0f))
                    {
                        on_movement_speed_changed(_movement_speed);
                    }
                    checkbox(Names::acceleration, _acceleration, on_camera_acceleration);
                    if (ImGui::SliderFloat(Names::acceleration_rate.c_str(), &_acceleration_rate, 0.0f, 1.0f))
                    {
                        on_camera_acceleration_rate(_acceleration_rate);
                    }
                    if (ImGui::Button(Names::reset_fov.c_str()))
                    {
                        _fov = 45;
                        on_camera_fov(_fov);
                    }
                    ImGui::SameLine();
                    if (ImGui::SliderFloat(Names::fov.c_str(), &_fov, 10, 145))
                    {
                        on_camera_fov(_fov);
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Minimap"))
                {
                    checkbox(Names::invert_map_controls, _invert_map_controls, on_invert_map_controls);
                    ImGui::Separator();

                    auto add_colour = [&](const std::string& name, auto&& flag)
                    {
                        if (ImGui::Button(("Reset##" + name).c_str()))
                        {
                            _colours.clear_colour(flag);
                            on_minimap_colours(_colours);
                        }
                        ImGui::SameLine();
                        DirectX::SimpleMath::Color colour = _colours.colour(flag);
                        if (ImGui::ColorEdit4(name.c_str(), &colour.x))
                        {
                            _colours.set_colour(flag, colour);
                            on_minimap_colours(_colours);
                        }
                    };

                    auto add_special = [&](const std::string& name, auto&& type)
                    {
                        if (ImGui::Button(("Reset##" + name).c_str()))
                        {
                            _colours.clear_colour(type);
                            on_minimap_colours(_colours);
                        }
                        ImGui::SameLine();
                        DirectX::SimpleMath::Color colour = _colours.colour(type);
                        if (ImGui::ColorEdit4(name.c_str(), &colour.x))
                        {
                            _colours.set_colour(type, colour);
                            on_minimap_colours(_colours);
                        }
                    };

                    add_special("Default", MapColours::Special::Default);
                    add_colour("Portal", SectorFlag::Portal);
                    add_colour("Special Wall", SectorFlag::SpecialWall);
                    add_colour("Wall", SectorFlag::Wall);
                    add_special("Geometry Wall", MapColours::Special::GeometryWall);
                    add_colour("Trigger", SectorFlag::Trigger);
                    add_colour("Death", SectorFlag::Death);
                    add_colour("Minecart Left", SectorFlag::MinecartLeft);
                    add_colour("Minecart Right", SectorFlag::MinecartRight);
                    add_colour("Monkey Swing", SectorFlag::MonkeySwing);
                    add_colour("Climbable North", SectorFlag::ClimbableNorth);
                    add_colour("Climbable South", SectorFlag::ClimbableSouth);
                    add_colour("Climbable East", SectorFlag::ClimbableEast);
                    add_colour("Climbable West", SectorFlag::ClimbableWest);
                    add_special("No Space", MapColours::Special::NoSpace);
                    add_special("Room Above", MapColours::Special::RoomAbove);
                    add_special("Room Below", MapColours::Special::RoomBelow);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Route"))
                {
                    if (ImGui::ColorEdit3(Names::default_route_colour.c_str(), &_default_route_colour.r))
                    {
                        on_default_route_colour(_default_route_colour);
                    }

                    if (ImGui::ColorEdit3(Names::default_waypoint_colour.c_str(), &_default_waypoint_colour.r))
                    {
                        on_default_waypoint_colour(_default_waypoint_colour);
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Plugins"))
                {
                    if (ImGui::BeginTable("Directories", 3, 0, ImVec2(-1, -1)))
                    {
                        auto directories = _plugin_directories;
                        for (std::size_t i = 0; i < directories.size(); ++i)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            if (ImGui::Button(std::format("Remove##{}", i).c_str()))
                            {
                                _plugin_directories.erase(_plugin_directories.begin() + i);
                                on_plugin_directories(_plugin_directories);
                            }
                            ImGui::TableNextColumn();
                            if (ImGui::Button(std::format("Open##{}", i).c_str()))
                            {
                                _shell->open(to_utf16(directories[i]));
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text(directories[i].c_str());
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Button("Add"))
                        {
                            if (auto path = _dialogs->open_folder())
                            {
                                _plugin_directories.push_back(path.value());
                                on_plugin_directories(_plugin_directories);
                            }
                        }

                        ImGui::EndTable();
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void SettingsWindow::set_vsync(bool value)
    {
        _vsync = value;
    }

    void SettingsWindow::set_go_to_lara(bool value)
    {
        _go_to_lara = value;
    }

    void SettingsWindow::set_invert_map_controls(bool value)
    {
        _invert_map_controls = value;
    }

    void SettingsWindow::set_items_startup(bool value)
    {
        _items_startup = value;
    }

    void SettingsWindow::set_triggers_startup(bool value)
    {
        _triggers_startup = value;
    }

    void SettingsWindow::set_rooms_startup(bool value)
    {
        _rooms_startup = value;
    }

    void SettingsWindow::set_auto_orbit(bool value)
    {
        _auto_orbit = value;
    }

    void SettingsWindow::set_camera_acceleration(bool value)
    {
        _acceleration = value;
    }

    void SettingsWindow::set_camera_acceleration_rate(float value)
    {
        _acceleration_rate = value;
    }

    void SettingsWindow::toggle_visibility()
    {
        _visible = !_visible;
    }

    void SettingsWindow::set_sensitivity(float value)
    {
        _sensitivity = value;
    }

    void SettingsWindow::set_movement_speed(float value)
    {
        _movement_speed = value;
    }

    void SettingsWindow::set_invert_vertical_pan(bool value)
    {
        _invert_vertical_panning = value;
    }

    void SettingsWindow::set_camera_display_degrees(bool value)
    {
        _camera_display_degrees = value;
    }

    void SettingsWindow::set_randomizer_tools(bool value)
    {
        _randomizer_tools = value;
    }

    void SettingsWindow::set_max_recent_files(uint32_t value)
    {
        _max_recent_files = value;
    }

    void SettingsWindow::set_background_colour(const Colour& colour)
    {
        _colour[0] = colour.r;
        _colour[1] = colour.g;
        _colour[2] = colour.b;
    }

    void SettingsWindow::set_map_colours(const MapColours& colours)
    {
        _colours = colours;
    }

    void SettingsWindow::set_default_route_colour(const Colour& colour)
    {
        _default_route_colour = colour;
    }

    void SettingsWindow::set_default_waypoint_colour(const Colour& colour)
    {
        _default_waypoint_colour = colour;
    }

    void SettingsWindow::set_route_startup(bool value)
    {
        _route_startup = value;
    }

    void SettingsWindow::set_fov(float value)
    {
        _fov = value;
    }

    void SettingsWindow::set_camera_sink_startup(bool value)
    {
        _camera_sink_startup = value;
    }

    void SettingsWindow::set_plugin_directories(const std::vector<std::string>& directories)
    {
        _plugin_directories = directories;
    }
}
