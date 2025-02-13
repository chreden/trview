#include "LightsWindow.h"
#include "../trview_imgui.h"
#include <format>
#include "RowCounter.h"
#include "../Elements/ILevel.h"

namespace trview
{
    namespace
    {
        std::unordered_map<std::string, std::string> tips
        {
            { "Direction", "Direction is inverted in-game. 3D view shows correct direction." }
        };

        bool is_level_mismatch(auto&& e, auto&& level)
        {
            if (auto e_ptr = e.lock())
            {
                return e_ptr->level().lock() != level.lock();
            }
            return false;
        }
    }

    ILightsWindow::~ILightsWindow()
    {
    }

    LightsWindow::LightsWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
    }

    void LightsWindow::add_level(const std::weak_ptr<ILevel>& level)
    {
        if (const auto new_level = level.lock())
        {
            _sub_windows.push_back(
                {
                    ._clipboard = _clipboard,
                    ._level = level,
                    ._level_version = new_level->version()
                });

            auto& new_window = _sub_windows.back();
            new_window.setup_filters();
            new_window.set_lights(new_level->lights());

            new_window.on_light_selected += on_light_selected;
            new_window.on_scene_changed += on_scene_changed;
        }
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
        for (auto& sub_window : _sub_windows)
        {
            sub_window.update(delta);
        }
    }

    void LightsWindow::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        for (auto& sub_window : _sub_windows)
        {
            sub_window.set_selected_light(light);
        }
    }

    void LightsWindow::SubWindow::set_sync_light(bool value)
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

    void LightsWindow::SubWindow::render_lights_list() 
    {
        calculate_column_widths();
        if (ImGui::BeginChild(Names::light_list_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _auto_hider.check_focus();

            _filters.render();
            ImGui::SameLine();

            _track.render();
            ImGui::SameLine();
            bool sync_light = _sync_light;
            if (ImGui::Checkbox(Names::sync_light.c_str(), &sync_light))
            {
                set_sync_light(sync_light);
            }

            _auto_hider.render();

            auto filtered_lights =
                _all_lights |
                std::views::filter([&](auto&& light)
                    {
                        const auto light_ptr = light .lock();
                        return !(!light_ptr || (_track.enabled<Type::Room>() && light_ptr->room().lock() != _current_room.lock() || !_filters.match(*light_ptr)));
                    }) |
                std::views::transform([](auto&& light) { return light.lock(); }) |
                std::ranges::to<std::vector>();

            if (_auto_hider.apply(_all_lights, filtered_lights, _filters))
            {
                on_scene_changed();
            }

            RowCounter counter{ "lights", _all_lights.size() };
            if (ImGui::BeginTable(Names::lights_listbox.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, -counter.height())))
            {
                imgui_header_row(
                    {
                        { "#", _column_sizer.size(0) },
                        { "Room", _column_sizer.size(1) },
                        { "Type", _column_sizer.size(2) },
                        {.name = "Hide", .width = _column_sizer.size(3), .set_checked = [&](bool v)
                            {
                                std::ranges::for_each(filtered_lights, [=](auto&& light) { light->set_visible(!v); });
                                on_scene_changed();
                            }, .checked = std::ranges::all_of(filtered_lights, [](auto&& light) { return !light->visible(); })
                        }
                    });

                imgui_sort_weak(_all_lights,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(light_room(l), l.number()) < std::tuple(light_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& light : filtered_lights)
                {
                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_light.lock() && _selected_light.lock()->number() == light->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_light)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_light = false;
                    }

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}##{0}", light->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();

                        set_local_selected_light(light);
                        if (_sync_light)
                        {
                            on_light_selected(light);
                        }

                        _scroll_to_light = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(light_room(light)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(light->type()).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !light->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", light->number()).c_str(), &hidden))
                    {
                        light->set_visible(!hidden);
                        on_scene_changed();
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
                counter.render();
            }
        }
        ImGui::EndChild();
    }

    void LightsWindow::SubWindow::render_light_details()
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
                    auto add_stat = [&]<typename T>(const std::string& name, const T&& value, Colour colour = Colour::White)
                    {
                        const auto string_value = get_string(value);
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                        {
                            _clipboard->write(to_utf16(string_value));
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
                        ImGui::Text(string_value.c_str());
                        ImGui::PopStyleColor();
                    };

                    auto add_stat_with_condition = [&](const std::string& name, const auto& stat, const auto& condition)
                    {
                        if (condition(*selected_light))
                        {
                            add_stat(name, stat(*selected_light));
                        }
                    };

                    auto format_colour = [](const Colour& colour)
                    {
                        return std::format("R: {}, G: {}, B: {}", static_cast<int>(colour.r * 255), static_cast<int>(colour.g * 255), static_cast<int>(colour.b * 255));
                    };

                    add_stat("Type", to_string(selected_light->type()));
                    add_stat("#", selected_light->number());
                    add_stat("Room", light_room(selected_light));

                    if (has_colour(*selected_light))
                    {
                        add_stat("Colour", format_colour(selected_light->colour()), selected_light->colour());
                    }

                    if (has_position(*selected_light))
                    {
                        const auto pos = selected_light->position() * trlevel::Scale;
                        add_stat("Position", std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z));
                    }

                    if (has_direction(*selected_light))
                    {
                        const auto dir = selected_light->direction() * trlevel::Scale;
                        add_stat("Direction", std::format("{:.3f}, {:.3f}, {:.3f}", dir.x, dir.y, dir.z));
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
            if (ImGui::BeginTabBar("TabBar"))
            {
                int window_index = 0;
                for (auto& sub_window : _sub_windows)
                {
                    sub_window.render(window_index++);
                }
                ImGui::EndTabBar();
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

    void LightsWindow::SubWindow::set_local_selected_light(const std::weak_ptr<ILight>& light)
    {
        _selected_light = light;
    }

    void LightsWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        for (auto& sub_window : _sub_windows)
        {
            sub_window.set_current_room(room);
        }
    }

    void LightsWindow::SubWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        if (is_level_mismatch(room, _level))
        {
            return;
        }

        _current_room = room;
    }

    void LightsWindow::SubWindow::setup_filters()
    {
        _filters.clear_all_getters();
        std::set<std::string> available_types;
        for (const auto& light : _all_lights)
        {
            if (auto light_ptr = light.lock())
            {
                available_types.insert(to_string(light_ptr->type()));
            }
        }
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& light) { return to_string(light.type()); });
        _filters.add_getter<float>("#", [](auto&& light) { return static_cast<float>(light.number()); });
        _filters.add_getter<float>("Room", [](auto&& light) { return static_cast<float>(light_room(light)); });
        _filters.add_getter<float>("X", [](auto&& light) { return light.position().x * trlevel::Scale_X; }, has_position);
        _filters.add_getter<float>("Y", [](auto&& light) { return light.position().y * trlevel::Scale_Y; }, has_position);
        _filters.add_getter<float>("Z", [](auto&& light) { return light.position().z * trlevel::Scale_Z; }, has_position);
        _filters.add_getter<float>("Intensity", [](auto&& light) { return static_cast<float>(light.intensity()); }, has_intensity);
        _filters.add_getter<float>("Fade", [](auto&& light) { return static_cast<float>(light.fade()); }, has_fade);

        if (_level_version >= trlevel::LevelVersion::Tomb3)
        {
            _filters.add_getter<float>("R", [](auto&& light) { return std::floor(light.colour().r * 255.0f); }, has_colour);
            _filters.add_getter<float>("G", [](auto&& light) { return std::floor(light.colour().g * 255.0f); }, has_colour);
            _filters.add_getter<float>("B", [](auto&& light) { return std::floor(light.colour().b * 255.0f); }, has_colour);
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

    void LightsWindow::SubWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("Type__", 2);
        _column_sizer.measure("Hide____", 3);

        for (const auto& light : _all_lights)
        {
            if (auto light_ptr = light.lock())
            {
                _column_sizer.measure(std::format("{0}", light_ptr->number()), 0);
                _column_sizer.measure(std::to_string(light_room(*light_ptr)), 1);
                _column_sizer.measure(to_string(light_ptr->type()), 2);
            }
        }
    }

    void LightsWindow::SubWindow::set_lights(const std::vector<std::weak_ptr<ILight>>& lights)
    {
        _all_lights = lights;
        setup_filters();
        _force_sort = true;
        calculate_column_widths();
    }

    void LightsWindow::SubWindow::update(float delta)
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

    void LightsWindow::SubWindow::render(int index)
    {
        if (const auto& level = _level.lock())
        {
            if (ImGui::BeginTabItem(std::format("{}##{}", level->name(), index).c_str()))
            {
                render_lights_list();
                ImGui::SameLine();
                render_light_details();
                _force_sort = false;

                if (_tooltip_timer.has_value())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Copied");
                    ImGui::EndTooltip();
                }
                ImGui::EndTabItem();
            }
        }
    }

    void LightsWindow::SubWindow::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        if (is_level_mismatch(light, _level))
        {
            return;
        }

        _global_selected_light = light;
        if (_sync_light)
        {
            _scroll_to_light = true;
            set_local_selected_light(light);
        }
    }
}


