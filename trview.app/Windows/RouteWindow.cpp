#include "RouteWindow.h"
#include <trview.app/Routing/Route.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/IClipboard.h>
#include "../trview_imgui.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    const std::string RouteWindow::Names::colour = "colour";

    using namespace graphics;

    RouteWindow::RouteWindow(const trview::Window& parent, const std::shared_ptr<IClipboard>& clipboard, const std::shared_ptr<IDialogs>& dialogs,
        const std::shared_ptr<IFiles>& files, const std::shared_ptr<IShell>& shell)
        : _clipboard(clipboard), _dialogs(dialogs), _files(files), _window(parent)
    {
    }

    void RouteWindow::render_waypoint_list()
    {
        if (ImGui::BeginChild(Names::waypoint_list_panel.c_str(), ImVec2(150, 0), true))
        {
            auto colour = _route ? _route->colour() : Colour::Green;
            if (ImGui::ColorEdit3("##colour", &colour.r, ImGuiColorEditFlags_NoInputs))
            {
                on_colour_changed(colour);
            }
            ImGui::SameLine();
            if (ImGui::Button(Names::import_button.c_str()))
            {
                std::vector<IDialogs::FileFilter> filters{ { L"trview route", { L"*.tvr" } } };
                if (_randomizer_enabled)
                {
                    filters.push_back({ L"Randomizer Locations", { L"*.json" } });
                }

                const auto filename = _dialogs->open_file(L"Import route", filters, OFN_FILEMUSTEXIST);
                if (filename.has_value())
                {
                    on_route_import(filename.value().filename, filename.value().filter_index == 2);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(Names::export_button.c_str()))
            {
                std::vector<IDialogs::FileFilter> filters{ { L"trview route", { L"*.tvr" } } };
                uint32_t filter_index = 1;
                if (_randomizer_enabled)
                {
                    filters.push_back({ L"Randomizer Locations", { L"*.json" } });
                    filter_index = 2;
                }

                const auto filename = _dialogs->save_file(L"Export route", filters, filter_index);
                if (filename.has_value())
                {
                    on_route_export(filename.value().filename, filename.value().filter_index == 2);
                }
            }

            if (ImGui::BeginTable("##waypointslist", 2, ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (_route) 
                {
                    for (uint32_t i = 0; i < _route->waypoints(); ++i)
                    {
                        const auto& waypoint = _route->waypoint(i);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = _selected_index == i;
                        imgui_scroll_to_item(selected, _scroll_to_trigger);
                        if (ImGui::Selectable(std::to_string(i).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                        {
                            _selected_index = i;
                            on_waypoint_selected(i);
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(to_utf8(waypoint_type_to_string(waypoint.type())).c_str());
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    void RouteWindow::render_waypoint_details()
    {
        if (ImGui::BeginChild(Names::waypoint_details_panel.c_str(), ImVec2(), true))
        {
            if (_route && _selected_index < _route->waypoints())
            {
                auto& waypoint = _route->waypoint(_selected_index);

                if (ImGui::BeginTable(Names::waypoint_stats.c_str(), 2, 0, ImVec2(-1, 80)))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableNextRow();

                    auto add_stat = [&](const std::string& name, const std::string& value)
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _clipboard->write(to_utf16(value));
                            _tooltip_timer = 0.0f;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(value.c_str());
                    };

                    auto get_room_pos = [&waypoint, this]()
                    {
                        if (waypoint.room() < _all_rooms.size())
                        {
                            const auto room = _all_rooms[waypoint.room()].lock();
                            if (!room)
                            {
                                return waypoint.position();
                            }
                            const auto info = room->info();
                            const Vector3 bottom_left = Vector3(info.x, info.yBottom, info.z) / trlevel::Scale_X;
                            return waypoint.position() - bottom_left;
                        }
                        return waypoint.position();
                    };

                    auto position_text = [](const auto& pos)
                    {
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(0) <<
                            pos.x * trlevel::Scale_X << ", " <<
                            pos.y * trlevel::Scale_Y << ", " <<
                            pos.z * trlevel::Scale_Z;
                        return stream.str();
                    };

                    add_stat("Type", to_utf8(waypoint_type_to_string(waypoint.type())));
                    add_stat("Position", position_text(waypoint.position()));
                    add_stat("Room", std::to_string(waypoint.room()));
                    add_stat("Room Position", position_text(get_room_pos()));
                    ImGui::EndTable();
                }

                const std::string save_text = waypoint.has_save() ? "SAVEGAME.0" : Names::attach_save.c_str();
                if (ImGui::Button(save_text.c_str(), ImVec2(-24, 18)))
                {
                    if (!waypoint.has_save())
                    {
                        const auto filename = _dialogs->open_file(L"Select Save", { { L"Savegame File", { L"*.*" } } }, OFN_FILEMUSTEXIST);
                        if (filename.has_value())
                        {
                            // Load bytes from file.
                            try
                            {
                                const auto bytes = _files->load_file(filename.value().filename);
                                if (bytes.has_value() && !bytes.value().empty())
                                {
                                    waypoint.set_save_file(bytes.value());
                                    _route->set_unsaved(true);
                                }
                            }
                            catch (...)
                            {
                                _dialogs->message_box(_window, L"Failed to attach save", L"Error", IDialogs::Buttons::OK);
                            }
                        }
                    }
                    else
                    {
                        const auto filename = _dialogs->save_file(L"Export Save", { { L"Savegame File", { L"*.*" } } }, 1);
                        if (filename.has_value())
                        {
                            try
                            {
                                _files->save_file(filename.value().filename, waypoint.save_file());
                            }
                            catch (...)
                            {
                                _dialogs->message_box(_window, L"Failed to export save", L"Error", IDialogs::Buttons::OK);
                            }
                        }
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(Names::clear_save.c_str(), ImVec2(-1,0)))
                {
                    if (waypoint.has_save())
                    {
                        waypoint.set_save_file({});
                        _route->set_unsaved(true);
                    }
                }

                if (ImGui::Button(Names::delete_waypoint.c_str(), ImVec2(-1, 0)))
                {
                    on_waypoint_deleted(_selected_index);
                }

                ImGui::Text("Notes");
                std::string notes = to_utf8(waypoint.notes());
                if (ImGui::InputTextMultiline(Names::notes.c_str(), &notes, ImVec2(-1, 200)))
                {
                    waypoint.set_notes(to_utf16(notes));
                    _route->set_unsaved(true);
                }

                if (_randomizer_enabled)
                {
                    ImGui::Text("Randomizer");
                    load_randomiser_settings(waypoint);
                }
            }
        }
        ImGui::EndChild();
    }

    bool RouteWindow::render_route_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(420, 500));
        if (ImGui::Begin("Route", &stay_open))
        {
            render_waypoint_list();
            ImGui::SameLine();
            render_waypoint_details();

            if (_tooltip_timer.has_value())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Copied");
                ImGui::EndTooltip();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::ShowStackToolWindow();

        return stay_open;
    }

    void RouteWindow::render()
    {
        if (!render_route_window())
        {
            on_window_closed();
            return;
        }
    }

    void RouteWindow::set_route(IRoute* route) 
    {
        _route = route;
        _selected_index = 0u;
    }

    void RouteWindow::select_waypoint(uint32_t index)
    {
        _selected_index = index;
        _scroll_to_trigger = true;
    }

    /// Set the items to that are in the level.
    /// @param items The items to show.
    void RouteWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    void RouteWindow::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
    }

    void RouteWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
    }

    void RouteWindow::focus()
    {
        SetForegroundWindow(_window);
    }

    void RouteWindow::update(float delta)
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

    void RouteWindow::set_randomizer_enabled(bool value)
    {
        _randomizer_enabled = value;
    }

    void RouteWindow::set_randomizer_settings(const RandomizerSettings& settings)
    {
        _randomizer_settings = settings;
    }

    void RouteWindow::load_randomiser_settings(IWaypoint& waypoint)
    {
        auto waypoint_settings = waypoint.randomizer_settings();

        if (ImGui::BeginTable(Names::randomizer_flags.c_str(), 2))
        {
            for (const auto& b : _randomizer_settings.settings)
            {
                if (b.second.type == RandomizerSettings::Setting::Type::Boolean)
                {
                    ImGui::TableNextColumn();
                    auto value =
                        std::get<bool>(waypoint_settings.find(b.first) == waypoint_settings.end() ?
                            b.second.default_value : waypoint_settings[b.first]);
                    if (ImGui::Checkbox(b.second.display.c_str(), &value))
                    {
                        auto& waypoint = _route->waypoint(_selected_index);
                        auto settings = waypoint.randomizer_settings();
                        settings[b.first] = value;
                        waypoint.set_randomizer_settings(settings);
                        _route->set_unsaved(true);
                    }
                }
            }
            ImGui::EndTable();
        }

        for (const auto& b : _randomizer_settings.settings)
        {
            auto value = waypoint_settings.find(b.first) == waypoint_settings.end() ? b.second.default_value : waypoint_settings[b.first];
            switch (b.second.type)
            {
                case RandomizerSettings::Setting::Type::String:
                {
                    auto text = std::get<std::string>(value);
                    if (b.second.options.empty())
                    {
                        if (ImGui::InputText(b.second.display.c_str(), &text, ImGuiInputTextFlags_AutoSelectAll))
                        {
                            auto settings = waypoint.randomizer_settings();
                            settings[b.first] = text;
                            waypoint.set_randomizer_settings(settings);
                            _route->set_unsaved(true);
                        }
                    }
                    else
                    {
                        if (ImGui::BeginCombo(b.second.display.c_str(), text.c_str()))
                        {
                            for (const auto& option : b.second.options)
                            {
                                bool is_selected = std::get<std::string>(option) == text;
                                if (ImGui::Selectable(std::get<std::string>(option).c_str(), is_selected))
                                {
                                    auto settings = waypoint.randomizer_settings();
                                    settings[b.first] = option;
                                    waypoint.set_randomizer_settings(settings);
                                    _route->set_unsaved(true);
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                    break;
                }
                case RandomizerSettings::Setting::Type::Number:
                {
                    auto number = std::get<float>(value);
                    if (ImGui::InputFloat(b.second.display.c_str(), &number, ImGuiInputTextFlags_AutoSelectAll))
                    {
                        auto settings = waypoint.randomizer_settings();
                        settings[b.first] = number;
                        waypoint.set_randomizer_settings(settings);
                        _route->set_unsaved(true);
                    }
                    break;
                }
            }
        }
    }
}
