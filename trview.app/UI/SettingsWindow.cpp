#include "SettingsWindow.h"

namespace trview
{
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
                    checkbox(Names::invert_map_controls, _invert_map_controls, on_invert_map_controls);
                    checkbox(Names::items_startup, _items_startup, on_items_startup);
                    checkbox(Names::triggers_startup, _triggers_startup, on_triggers_startup);
                    checkbox(Names::rooms_startup, _rooms_startup, on_rooms_startup);
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
}
