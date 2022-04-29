#include "LightsWindow.h"
#include "../trview_imgui.h"

namespace trview
{
    LightsWindow::LightsWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        _tips["Direction"] = "Direction is inverted in-game. 3D view shows correct direction.";
        setup_filters();
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
        setup_filters();
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
            _filters.render();
            ImGui::SameLine();

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
                    if (_track_room && light->room() != _current_room || !_filters.match(*light))
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_light.lock() && _selected_light.lock()->number() == light->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_light)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_light = false;
                    }

                    if (ImGui::Selectable((std::to_string(light->number()) + std::string("##") + std::to_string(light->number())).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav | ImGuiTableFlags_SizingFixedFit))
                    {
                        scroller.fix_scroll();

                        set_local_selected_light(light);
                        if (_sync_light)
                        {
                            on_light_selected(light);
                        }

                        _scroll_to_light = false;
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

                    auto add_stat_with_condition = [&](const std::string& name, const auto& stat, const auto& condition)
                    {
                        if (condition(*selected_light))
                        {
                            add_stat(name, std::to_string(stat(*selected_light)));
                        }
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

                    if (has_colour(*selected_light))
                    {
                        add_stat("Colour", format_colour(selected_light->colour()), selected_light->colour());
                    }

                    if (has_position(*selected_light))
                    {
                        add_stat("Position", position_text(selected_light->position()));
                    }

                    if (has_direction(*selected_light))
                    {
                        add_stat("Direction", direction_text(selected_light->direction()));
                    }

                    add_stat_with_condition("Intensity", intensity, has_intensity);
                    add_stat_with_condition("Fade", fade, has_fade);
                    add_stat_with_condition("Hotspot", hotspot, has_hotspot);
                    add_stat_with_condition("Falloff", falloff, has_falloff);
                    add_stat_with_condition("Falloff Angle", falloff_angle, has_falloff_angle);
                    add_stat_with_condition("Length", length, has_length);
                    add_stat_with_condition("Cutoff", cutoff, has_cutoff);
                    add_stat_with_condition("Rad In", rad_in, has_rad_in);
                    add_stat_with_condition("Rad Out", rad_out, has_rad_out);
                    add_stat_with_condition("Range", range, has_range);
                    add_stat_with_condition("Density", density, has_density);
                    add_stat_with_condition("Radius", radius, has_radius);
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

    void LightsWindow::setup_filters()
    {
        _filters.clear_all_getters();
        std::set<std::string> available_types;
        for (const auto& light : _all_lights)
        {
            if (auto light_ptr = light.lock())
            {
                available_types.insert(to_utf8(light_type_name(light_ptr->type())));
            }
        }
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& light) { return to_utf8(light_type_name(light.type())); });
        _filters.add_getter<float>("#", [](auto&& light) { return light.number(); });
        _filters.add_getter<float>("Room", [](auto&& light) { return light.room(); });
        _filters.add_getter<float>("X", [](auto&& light) { return light.position().x * trlevel::Scale_X; }, has_position);
        _filters.add_getter<float>("Y", [](auto&& light) { return light.position().y * trlevel::Scale_Y; }, has_position);
        _filters.add_getter<float>("Z", [](auto&& light) { return light.position().z * trlevel::Scale_Z; }, has_position);
        _filters.add_getter<float>("Intensity", [](auto&& light) { return light.intensity(); }, has_intensity);
        _filters.add_getter<float>("Fade", [](auto&& light) { return light.fade(); }, has_fade);

        if (_level_version >= trlevel::LevelVersion::Tomb3)
        {
            _filters.add_getter<float>("R", [](auto&& light) { return static_cast<int>(light.colour().r * 255.0f); }, has_colour);
            _filters.add_getter<float>("G", [](auto&& light) { return static_cast<int>(light.colour().g * 255.0f); }, has_colour);
            _filters.add_getter<float>("B", [](auto&& light) { return static_cast<int>(light.colour().b * 255.0f); }, has_colour);
            _filters.add_getter<float>("DX", [](auto&& light) { return light.direction().x * trlevel::Scale_X; }, has_direction);
            _filters.add_getter<float>("DY", [](auto&& light) { return light.direction().y * trlevel::Scale_Y; }, has_direction);
            _filters.add_getter<float>("DZ", [](auto&& light) { return light.direction().z * trlevel::Scale_Z; }, has_direction);
        }

        if (_level_version == trlevel::LevelVersion::Tomb4)
        {
            _filters.add_getter<float>("Length", [](auto&& light) { return length(light); }, has_length);
            _filters.add_getter<float>("Cutoff", [](auto&& light) { return cutoff(light); }, has_cutoff);
        }

        if (_level_version >= trlevel::LevelVersion::Tomb4)
        {
            _filters.add_getter<float>("Hotspot", [](auto&& light) { return hotspot(light); }, has_hotspot);
            _filters.add_getter<float>("Falloff", [](auto&& light) { return falloff(light); }, has_falloff);
            _filters.add_getter<float>("Falloff Angle", [](auto&& light) { return falloff_angle(light); }, has_falloff_angle);
            _filters.add_getter<float>("Density", [](auto&& light) { return density(light); }, has_density);
            _filters.add_getter<float>("Radius", [](auto&& light) { return radius(light); }, has_radius);
        }

        if (_level_version >= trlevel::LevelVersion::Tomb5)
        {
            _filters.add_getter<float>("Rad In", [](auto&& light) { return rad_in(light); }, has_rad_in);
            _filters.add_getter<float>("Rad Out", [](auto&& light) { return rad_out(light); }, has_rad_out);
            _filters.add_getter<float>("Range", [](auto&& light) { return range(light); }, has_range);
        }
    }
}


