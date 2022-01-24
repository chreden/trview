#include "SettingsWindow.h"
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

using namespace trview::ui;

namespace trview
{
    const std::string SettingsWindow::Names::vsync = "VSync";
    const std::string SettingsWindow::Names::go_to_lara = "GoToLara";
    const std::string SettingsWindow::Names::invert_map_controls = "InvertMapControls";
    const std::string SettingsWindow::Names::items_startup = "ItemsStartup";
    const std::string SettingsWindow::Names::triggers_startup = "TriggersStartup";
    const std::string SettingsWindow::Names::rooms_startup = "RoomsStartup";
    const std::string SettingsWindow::Names::auto_orbit = "AutoOrbit";
    const std::string SettingsWindow::Names::invert_vertical_pan = "InvertVerticalPan";
    const std::string SettingsWindow::Names::camera_display_degrees = "CameraDisplayDegrees";
    const std::string SettingsWindow::Names::randomizer_tools = "RandomizerTools";
    const std::string SettingsWindow::Names::max_recent_files = "MaxRecentFiles";
    const std::string SettingsWindow::Names::sensitivity = "Sensitivity";
    const std::string SettingsWindow::Names::movement_speed = "MovementSpeed";
    const std::string SettingsWindow::Names::acceleration = "Acceleration";
    const std::string SettingsWindow::Names::acceleration_rate = "AccelerationRate";
    const std::string SettingsWindow::Names::close = "Close";

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

        if (ImGui::Begin("Settings", &_visible))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("General"))
                {
                    checkbox("Vsync", _vsync, on_vsync);
                    checkbox("Select Lara when level is opened", _go_to_lara, on_go_to_lara);
                    checkbox("Invert map controls", _invert_map_controls, on_invert_map_controls);
                    checkbox("Open Items Window at startup", _items_startup, on_items_startup);
                    checkbox("Open Triggers Window at startup", _triggers_startup, on_triggers_startup);
                    checkbox("Open Rooms Window at startup", _rooms_startup, on_rooms_startup);
                    checkbox("Enable Randomizer Tools", _randomizer_tools, on_randomizer_tools);
                    if (ImGui::InputInt("Recent Files", &_max_recent_files))
                    {
                        _max_recent_files = std::max(0, _max_recent_files);
                        on_max_recent_files(_max_recent_files);
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Camera"))
                {
                    checkbox("Invert vertical panning", _invert_vertical_panning, on_invert_vertical_pan);
                    checkbox("Use degrees for camera angle display", _camera_display_degrees, on_camera_display_degrees);
                    checkbox("Switch to orbit on selection", _auto_orbit, on_auto_orbit);
                    if (ImGui::SliderFloat("Sensitivity", &_sensitivity, 0.0f, 1.0f))
                    {
                        on_sensitivity_changed(_sensitivity);
                    }
                    if (ImGui::SliderFloat("Movement Speed", &_movement_speed, 0.0f, 1.0f))
                    {
                        on_movement_speed_changed(_movement_speed);
                    }
                    checkbox("Acceleration", _acceleration, on_camera_acceleration);
                    if (ImGui::SliderFloat("Acceleration Rate", &_acceleration_rate, 0.0f, 1.0f))
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
}
