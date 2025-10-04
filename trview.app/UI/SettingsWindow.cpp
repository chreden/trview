#include "SettingsWindow.h"
#include "../Graphics/ITextureStorage.h"

namespace trview
{
    ISettingsWindow::~ISettingsWindow()
    {
    }

    SettingsWindow::SettingsWindow(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IShell>& shell, const std::shared_ptr<IFonts>& fonts, const std::shared_ptr<ITextureStorage>& texture_storage)
        : _dialogs(dialogs), _shell(shell), _fonts(fonts)
    {
        _linear_texture = texture_storage->lookup("texture_filtering_linear");
        _point_texture = texture_storage->lookup("texture_filtering_point");
    }

    void SettingsWindow::checkbox(const std::string& name, bool& setting)
    {
        if (ImGui::Checkbox(name.c_str(), &setting))
        {
            on_settings(_settings);
        }
    }

    void SettingsWindow::slider(const std::string& name, float& value, float min, float max)
    {
        if (ImGui::SliderFloat(name.c_str(), &value, min, max))
        {
            on_settings(_settings);
        }
    }

    void SettingsWindow::render()
    {
        if (!_visible)
        {
            return;
        }

        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Settings", &_visible, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("General"))
                {
                    checkbox(Names::go_to_lara, _settings.go_to_lara);
                    checkbox(Names::items_startup, _settings.items_startup);
                    checkbox(Names::triggers_startup, _settings.triggers_startup);
                    checkbox(Names::rooms_startup, _settings.rooms_startup);
                    checkbox(Names::route_startup, _settings.route_startup);
                    checkbox(Names::camera_sink_startup, _settings.camera_sink_startup);
                    checkbox(Names::statics_startup, _settings.statics_startup);
                    checkbox(Names::randomizer_tools, _settings.randomizer_tools);
                    if (ImGui::InputInt(Names::max_recent_files.c_str(), &_settings.max_recent_files))
                    {
                        _settings.max_recent_files = std::max(0, _settings.max_recent_files);
                        on_settings(_settings);
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Visuals"))
                {
                    checkbox(Names::vsync, _settings.vsync);
                    if (ImGui::Checkbox(Names::linear_filtering.c_str(), &_settings.linear_filtering))
                    {
                        on_settings(_settings);
                        on_linear_filtering(_settings.linear_filtering);
                    }
                    show_texture_filtering_window();

                    Colour colour = _settings.background_colour;
                    float background_colour[3] = { colour.r, colour.g, colour.b };
                    if (ImGui::ColorEdit3(Names::background_colour.c_str(), &background_colour[0]))
                    {
                        _settings.background_colour = Colour(1.0f, background_colour[0], background_colour[1], background_colour[2]);
                        on_settings(_settings);
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Fonts"))
                {
                    if (ImGui::BeginTable("Fonts List", 3, ImGuiTableFlags_SizingFixedFit))
                    {
                        for (const auto font : _fonts->fonts())
                        {
                            ImGui::TableNextRow();

                            ImGui::TableNextColumn();
                            ImGui::AlignTextToFramePadding();
                            ImGui::Text(font.first.c_str());
                            ImGui::TableNextColumn();
                            if (ImGui::BeginCombo(std::format("##{}", font.first).c_str(), font.second.name.c_str(), ImGuiComboFlags_WidthFitPreview))
                            {
                                for (const auto& f : _all_fonts)
                                {
                                    bool is_selected = font.second.name == f.name;
                                    if (ImGui::Selectable(f.name.c_str(), is_selected))
                                    {
                                        auto selected = font.second;
                                        selected.name = f.name;
                                        selected.filename = f.filename;
                                        on_font(font.first, selected);
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            ImGui::TableNextColumn();
                            int size = font.second.size;
                            if (ImGui::InputInt(std::format("Font Size##{}", font.first).c_str(), &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
                            {
                                auto selected = font.second;
                                selected.size = size;
                                on_font(font.first, selected);
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Camera"))
                {
                    checkbox(Names::invert_vertical_pan, _settings.invert_vertical_pan);
                    checkbox(Names::camera_display_degrees, _settings.camera_display_degrees);
                    checkbox(Names::auto_orbit, _settings.auto_orbit);
                    slider(Names::sensitivity, _settings.camera_sensitivity, 0.0f, 1.0f);
                    slider(Names::movement_speed, _settings.camera_movement_speed, 0.0f, 1.0f);
                    checkbox(Names::acceleration, _settings.camera_acceleration);
                    slider(Names::acceleration_rate, _settings.camera_acceleration_rate, 0.0f, 1.0f);
                    if (ImGui::Button(Names::reset_fov.c_str()))
                    {
                        _settings.fov = 45;
                        on_settings(_settings);
                    }
                    ImGui::SameLine();
                    slider(Names::fov, _settings.fov, 10, 145);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Minimap"))
                {
                    checkbox(Names::invert_map_controls, _settings.invert_map_controls);
                    ImGui::Separator();

                    auto add_colour = [&](const std::string& name, auto&& flag)
                    {
                        if (ImGui::Button(("Reset##" + name).c_str()))
                        {
                            _settings.map_colours.clear_colour(flag);
                            on_settings(_settings);
                        }
                        ImGui::SameLine();
                        DirectX::SimpleMath::Color colour = _settings.map_colours.colour(flag);
                        if (ImGui::ColorEdit4(name.c_str(), &colour.x))
                        {
                            _settings.map_colours.set_colour(flag, colour);
                            on_settings(_settings);
                        }
                    };

                    auto add_special = [&](const std::string& name, auto&& type)
                    {
                        if (ImGui::Button(("Reset##" + name).c_str()))
                        {
                            _settings.map_colours.clear_colour(type);
                            on_settings(_settings);
                        }
                        ImGui::SameLine();
                        DirectX::SimpleMath::Color colour = _settings.map_colours.colour(type);
                        if (ImGui::ColorEdit4(name.c_str(), &colour.x))
                        {
                            _settings.map_colours.set_colour(type, colour);
                            on_settings(_settings);
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
                    if (ImGui::ColorEdit3(Names::default_route_colour.c_str(), &_settings.route_colour.r))
                    {
                        on_settings(_settings);
                    }

                    if (ImGui::ColorEdit3(Names::default_waypoint_colour.c_str(), &_settings.waypoint_colour.r))
                    {
                        on_settings(_settings);
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void SettingsWindow::toggle_visibility()
    {
        _visible = !_visible;
        if (_visible)
        {
            _all_fonts = _fonts->list_fonts();
        }
    }

    void SettingsWindow::set_settings(const UserSettings& settings)
    {
        _settings = settings;
    }

    void SettingsWindow::show_texture_filtering_window()
    {
        ImGui::SameLine();
        if (ImGui::Button("?"))
        {
            ImGui::OpenPopup("Texture Filtering");
            _showing_filtering_popup = true;
        }

        if (ImGui::BeginPopupModal("Texture Filtering", &_showing_filtering_popup, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Enable Linear Filtering to use the smooth texture appearance of the classics on PC. This may also cause some seams to appear as they did in the game.");
            ImGui::Text("Disable this to use point filtering which doesn't have seams/tiling issues and is how the game appeared on PlayStation and PC without linear enabled.");
            ImGui::NewLine();
            if (ImGui::BeginTable("Comparison", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x * 0.5f - ImGui::CalcTextSize("Point Filtering").x * 0.5f);
                ImGui::Text("Point Filtering");
                ImGui::TableNextColumn();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x * 0.5f - ImGui::CalcTextSize("Linear Filtering").x * 0.5f);
                ImGui::Text("Linear Filtering");
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Image(_point_texture.view().Get(), ImVec2(512, 512));
                ImGui::TableNextColumn();
                ImGui::Image(_linear_texture.view().Get(), ImVec2(512, 512));
                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }
    }
}
