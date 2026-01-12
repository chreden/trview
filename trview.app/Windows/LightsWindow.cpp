#include "LightsWindow.h"
#include "../trview_imgui.h"
#include <format>
#include "RowCounter.h"
#include "../Messages/Messages.h"
#include "../Elements/ILevel.h"

namespace trview
{
    LightsWindow::LightsWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging)
        : _clipboard(clipboard), _messaging(messaging)
    {
        _tips["Direction"] = "Direction is inverted in-game. 3D view shows correct direction.";
        setup_filters();

        _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->lights_window_columns = _filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };
    }

    void LightsWindow::clear_selected_light()
    {
        _selected_light.reset();
    }

    void LightsWindow::render()
    {
        if (!_settings)
        {
            messages::get_settings(_messaging, weak_from_this());
        }

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
        _filters.force_sort();
    }

    void LightsWindow::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        _global_selected_light = light;
        if (_sync_light)
        {
            _filters.scroll_to_item();
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
            _filters.scroll_to_item();
            if (_sync_light && _global_selected_light.lock())
            {
                set_selected_light(_global_selected_light);
            }
        }
    }

    void LightsWindow::render_lights_list() 
    {
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
            ImGui::SameLine();
            _filters.render_settings();

            auto filtered_lights =
                _all_lights |
                std::views::filter([&](auto&& light)
                    {
                        const auto light_ptr = light .lock();
                        return !(!light_ptr || (_track.enabled<Type::Room>() && light_ptr->room().lock() != _current_room.lock() || !_filters.match(*light_ptr)));
                    }) |
                std::views::transform([](auto&& light) { return light.lock(); }) |
                std::ranges::to<std::vector>();

            _auto_hider.apply(_all_lights, filtered_lights, _filters);

            RowCounter counter{ "light", _all_lights.size() };
            _filters.render_table(filtered_lights, _all_lights, _selected_light, counter,
                [&](auto&& light)
                {
                    set_local_selected_light(light);
                    if (_sync_light)
                    {
                        messages::send_select_light(_messaging, light);
                    }
                }, default_hide(filtered_lights));
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

    void LightsWindow::set_current_room(const std::weak_ptr<IRoom>& room)
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
                available_types.insert(to_string(light_ptr->type()));
            }
        }
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& light) { return to_string(light.type()); });
        _filters.add_getter<int>("#", [](auto&& light) { return static_cast<int>(light.number()); });
        _filters.add_getter<int>("Room", [](auto&& light) { return static_cast<int>(light_room(light)); });
        _filters.add_getter<int>("X", [](auto&& light) { return static_cast<int>(light.position().x * trlevel::Scale_X); }, has_position);
        _filters.add_getter<int>("Y", [](auto&& light) { return static_cast<int>(light.position().y * trlevel::Scale_Y); }, has_position);
        _filters.add_getter<int>("Z", [](auto&& light) { return static_cast<int>(light.position().z * trlevel::Scale_Z); }, has_position);
        _filters.add_getter<int>("Intensity", [](auto&& light) { return static_cast<int>(light.intensity()); }, has_intensity);
        _filters.add_getter<int>("Fade", [](auto&& light) { return static_cast<int>(light.fade()); }, has_fade);
        _filters.add_getter<bool>("Hide", [](auto&& light) { return !light.visible(); }, EditMode::ReadWrite);
        _filters.add_getter<bool>("In Visible Room", [](auto&& light)
            {
                if (const auto level = light.level().lock())
                {
                    return level->is_in_visible_set(light.room());
                }
                return false;
            });

        if (_level_version >= trlevel::LevelVersion::Tomb3)
        {
            _filters.add_getter<int>("R", [](auto&& light) { return static_cast<int>(std::floor(light.colour().r * 255.0f)); }, has_colour);
            _filters.add_getter<int>("G", [](auto&& light) { return static_cast<int>(std::floor(light.colour().g * 255.0f)); }, has_colour);
            _filters.add_getter<int>("B", [](auto&& light) { return static_cast<int>(std::floor(light.colour().b * 255.0f)); }, has_colour);
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

    void LightsWindow::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
            if (!_columns_set)
            {
                _filters.set_columns(_settings->lights_window_columns);
                _columns_set = true;
            }
        }
        else if (auto selected_light = messages::read_select_light(message))
        {
            set_selected_light(selected_light.value());
        }
        else if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                clear_selected_light();
                set_lights(level_ptr->lights());
                set_level_version(level_ptr->version());
            }
        }
        else if (auto selected_room = messages::read_select_room(message))
        {
            set_current_room(selected_room.value());
        }
    }

    void LightsWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
        messages::get_selected_room(_messaging, weak_from_this());
        messages::get_selected_light(_messaging, weak_from_this());
    }

    std::string LightsWindow::type() const
    {
        return "Lights";
    }

    std::string LightsWindow::title() const
    {
        return _id;
    }
}
