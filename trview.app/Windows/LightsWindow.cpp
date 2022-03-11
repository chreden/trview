#include "LightsWindow.h"
#include "../trview_imgui.h"

namespace trview
{
    LightsWindow::LightsWindow(const std::shared_ptr<IClipboard>& clipboard) 
        : _clipboard(clipboard)
    {
        _tips["Direction"] = "Direction is inverted in-game. 3D view shows correct direction.";
    }

    void LightsWindow::clear_selected_light()
    {
        _selected_light.reset();
    }

    void LightsWindow::render()
    {
        if (!render_lights_window())
        {
            on_window_closed();
            return;
        }
    }
    
    void LightsWindow::update(float delta)
    {
        if (_tooltip_timer.has_value())
        {
            _tooltip_timer = _tooltip_timer.value() + delta;
            if (_tooltip_timer.value() > 0.6f)
            {
                _tooltip_timer.reset();
            }
        }
    }

    void LightsWindow::set_lights(const std::vector<std::weak_ptr<ILight>>& lights)
    {
        _all_lights = lights;
    }

    void LightsWindow::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        _global_selected_light = light;
        if (_sync_light)
        {
            _scroll_to_light = true;
            set_local_selected_light(light);
        }
    }

    void LightsWindow::update_lights(const std::vector<std::weak_ptr<ILight>>& lights)
    {
        set_lights(lights);
        if (_track_room)
        {
            set_current_room(_current_room);
        }
    }

    void LightsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void LightsWindow::set_sync_light(bool value)
    {
        if (_sync_light != value)
        {
            _sync_light = value;
            _scroll_to_light = true;
            if (_sync_light && _global_selected_light.lock())
            {
                set_selected_light(_global_selected_light);
            }
        }
    }

    void LightsWindow::set_track_room(bool value)
    {
        _track_room = value;
    }

    void LightsWindow::render_lights_list() 
    {
        if (ImGui::BeginChild(Names::light_list_panel.c_str(), ImVec2(230, 0), true))
        {
            bool track_room = _track_room;
            if (ImGui::Checkbox(Names::track_room.c_str(), &track_room))
            {
                set_track_room(track_room);
            }
            ImGui::SameLine();
            bool sync_light = _sync_light;
            if (ImGui::Checkbox(Names::sync_light.c_str(), &sync_light))
            {
                set_sync_light(sync_light);
            }

            if (ImGui::BeginTable(Names::lights_listbox.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort_weak(_all_lights,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return l.room() < r.room(); },
                        [](auto&& l, auto&& r) { return l.type() < r.type(); },
                        [](auto&& l, auto&& r) { return l.visible() < r.visible(); }
                    });

                for (const auto& light_ptr : _all_lights)
                {
                    const auto& light = light_ptr.lock();
                    if (_track_room && light->room() != _current_room)
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_light.lock() && _selected_light.lock()->number() == light->number();
                    imgui_scroll_to_item(selected, _scroll_to_light);
                    if (ImGui::Selectable((std::to_string(light->number()) + std::string("##") + std::to_string(light->number())).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav | ImGuiTableFlags_SizingFixedFit))
                    {
                        set_local_selected_light(light);
                        if (_sync_light)
                        {
                            on_light_selected(light);
                        }
                    }
                    ImGui::SetItemAllowOverlap();
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(light->room()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_utf8(light_type_name(light->type())).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !light->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox((std::string("##hide-") + std::to_string(light->number())).c_str(), &hidden))
                    {
                        on_light_visibility(light, !hidden);
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    void LightsWindow::render_light_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            ImGui::Text("Light Details");
            if (ImGui::BeginTable(Names::stats_listbox.c_str(), 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                auto selected_light = _selected_light.lock();
                if (selected_light)
                {
                    auto add_stat = [&](const std::string& name, const std::string& value, Colour colour = Colour::White)
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                        {
                            _clipboard->write(to_utf16(value));
                            _tooltip_timer = 0.0f;
                        }
                        if (_level_version == trlevel::LevelVersion::Tomb4 && ImGui::IsItemHovered() && _tips.find(name) != _tips.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text(_tips[name].c_str());
                            ImGui::EndTooltip();
                        }
                        ImGui::TableNextColumn();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colour.r, colour.g, colour.b, colour.a));
                        ImGui::Text(value.c_str());
                        ImGui::PopStyleColor();
                    };

                    auto format_colour = [](const Colour& colour)
                    {
                        return "R:" + std::to_string(static_cast<int>(colour.r * 255)) +
                            " G:" + std::to_string(static_cast<int>(colour.g * 255)) +
                            " B:" + std::to_string(static_cast<int>(colour.b * 255));
                    };

                    auto position_text = [](const auto& vec)
                    {
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(0) <<
                            vec.x * trlevel::Scale_X << ", " <<
                            vec.y * trlevel::Scale_Y << ", " <<
                            vec.z * trlevel::Scale_Z;
                        return stream.str();
                    };

                    auto direction_text = [](const auto& vec)
                    {
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(3) <<
                            vec.x * trlevel::Scale_X << ", " <<
                            vec.y * trlevel::Scale_Y << ", " <<
                            vec.z * trlevel::Scale_Z;
                        return stream.str();
                    };

                    add_stat("Type", to_utf8(light_type_name(selected_light->type())));
                    add_stat("#", std::to_string(selected_light->number()));
                    add_stat("Room", std::to_string(selected_light->room()));

                    if (!(_level_version < trlevel::LevelVersion::Tomb3 || selected_light->type() == trlevel::LightType::FogBulb && _level_version == trlevel::LevelVersion::Tomb4))
                    {
                        add_stat("Colour", format_colour(selected_light->colour()), selected_light->colour());
                    }
                    if (selected_light->type() != trlevel::LightType::Sun)
                    {
                        add_stat("Position", position_text(selected_light->position()));
                    }

                    if (selected_light->type() == trlevel::LightType::Sun || selected_light->type() == trlevel::LightType::Spot)
                    {
                        add_stat("Direction", direction_text(selected_light->direction()));
                    }

                    if (_level_version < trlevel::LevelVersion::Tomb5 && selected_light->type() != trlevel::LightType::Sun)
                    {
                        add_stat("Intensity", std::to_string(selected_light->intensity()));
                    }

                    if (_level_version < trlevel::LevelVersion::Tomb4 && selected_light->type() != trlevel::LightType::Sun)
                    {
                        add_stat("Fade", std::to_string(selected_light->fade()));
                    }

                    if (_level_version >= trlevel::LevelVersion::Tomb4)
                    {
                        switch (selected_light->type())
                        {
                            case trlevel::LightType::Spot:
                            {
                                float hotspot = DirectX::XMConvertToDegrees(std::acosf(selected_light->in()));
                                float falloff = DirectX::XMConvertToDegrees(std::acosf(selected_light->out()));

                                add_stat("Hotspot", std::to_string(hotspot));
                                add_stat("Falloff Angle", std::to_string(falloff));
                                if (_level_version == trlevel::LevelVersion::Tomb4)
                                {
                                    add_stat("Length", std::to_string(selected_light->length()));
                                    add_stat("Cutoff", std::to_string(selected_light->cutoff()));
                                }
                                else
                                {
                                    float rad_in = DirectX::XMConvertToDegrees(selected_light->rad_in() * 0.5f);
                                    float rad_out = DirectX::XMConvertToDegrees(selected_light->rad_out() * 0.5f);

                                    add_stat("Rad In", std::to_string(rad_in));
                                    add_stat("Rad Out", std::to_string(rad_out));
                                    add_stat("Range", std::to_string(selected_light->range()));
                                }
                                break;
                            }
                            case trlevel::LightType::Point:
                            case trlevel::LightType::Shadow:
                            {
                                add_stat("Hotspot", std::to_string(selected_light->in()));
                                add_stat("Falloff", std::to_string(selected_light->out()));
                                break;
                            }
                            case trlevel::LightType::FogBulb:
                            {
                                add_stat("Density", std::to_string(selected_light->density()));
                                add_stat("Radius", std::to_string(selected_light->radius()));
                                break;
                            }
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    bool LightsWindow::render_lights_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(530, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_lights_list();
            ImGui::SameLine();
            render_light_details();

            if (_tooltip_timer.has_value())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Copied");
                ImGui::EndTooltip();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void LightsWindow::set_number(int32_t number)
    {
        _id = "Lights " + std::to_string(number);
    }

    void LightsWindow::set_local_selected_light(const std::weak_ptr<ILight>& light)
    {
        _selected_light = light;
    }

    void LightsWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
    }
}


