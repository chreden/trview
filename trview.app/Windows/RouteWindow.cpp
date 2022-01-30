#include "RouteWindow.h"
#include <trview.app/Routing/Route.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/IClipboard.h>
#include <external/imgui/misc/cpp/imgui_stdlib.h>

namespace trview
{
    using namespace DirectX::SimpleMath;

    const std::string RouteWindow::Names::colour = "colour";
    const std::string RouteWindow::Names::waypoints = "waypoints";
    const std::string RouteWindow::Names::delete_waypoint = "delete_waypoint";
    const std::string RouteWindow::Names::export_button = "export_button";
    const std::string RouteWindow::Names::import_button = "import_button";
    const std::string RouteWindow::Names::clear_save = "clear_save";
    const std::string RouteWindow::Names::notes_area = "notes_area";
    const std::string RouteWindow::Names::select_save_button = "select_save_button";
    const std::string RouteWindow::Names::waypoint_stats = "waypoint_stats";
    const std::string RouteWindow::Names::randomizer_group = "randomizer_group";
    const std::string RouteWindow::Names::randomizer_area = "randomizer_area";

    using namespace graphics;

    RouteWindow::RouteWindow(const trview::Window& parent, const std::shared_ptr<IClipboard>& clipboard, const std::shared_ptr<IDialogs>& dialogs,
        const std::shared_ptr<IFiles>& files, const std::shared_ptr<IShell>& shell)
        : _clipboard(clipboard), _dialogs(dialogs), _files(files), _shell(shell), _window(parent)
    {
    }

    bool RouteWindow::render_host()
    {
        bool stay_open = true;
        ImGui::Begin("Route", &stay_open);
        ImGuiID dockspaceID = ImGui::GetID("dockspace");
        if (!ImGui::DockBuilderGetNode(dockspaceID))
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::DockBuilderRemoveNode(dockspaceID);
            ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar);
            ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

            ImGuiID dock_main_id = dockspaceID;
            ImGui::DockBuilderDockWindow("WaypointList", dock_main_id);
            ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, NULL, &dock_main_id);
            ImGui::DockBuilderDockWindow("WaypointDetails", dock_id_right);

            ImGui::DockBuilderGetNode(dock_main_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            ImGui::DockBuilderGetNode(dock_id_right)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

            ImGui::DockBuilderFinish(dockspaceID);
        }
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);
        ImGui::End();
        return stay_open;
    }

    void RouteWindow::render_waypoint_list()
    {
        if (ImGui::Begin("WaypointList", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            auto colour = _route ? _route->colour() : Colour::Green;
            if (ImGui::ColorEdit3("##colour", &colour.r, ImGuiColorEditFlags_NoInputs))
            {
                on_colour_changed(colour);
            }
            ImGui::SameLine();
            if (ImGui::Button("Import"))
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
            if (ImGui::Button("Export"))
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

            if (ImGui::BeginTable("##waypointslist", 4, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
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
        ImGui::End();
    }

    void RouteWindow::render_waypoint_details()
    {
        if (ImGui::Begin("WaypointDetails", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            if (_route && _selected_index < _route->waypoints())
            {
                auto& waypoint = _route->waypoint(_selected_index);

                if (ImGui::BeginTable("##triggerstats", 2, 0, ImVec2(-1, 100)))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableNextRow();

                    auto add_stat = [&](const std::string& name, const std::string& value)
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _clipboard->write(_window, to_utf16(value));
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
                }
                ImGui::EndTable();

                const std::string save_text = waypoint.has_save() ? "SAVEGAME.0" : "Attach Save";
                if (ImGui::Button(save_text.c_str()))
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
                if (ImGui::Button("X"))
                {
                    if (waypoint.has_save())
                    {
                        waypoint.set_save_file({});
                        _route->set_unsaved(true);
                    }
                }

                if (ImGui::Button("Delete Waypoint"))
                {
                    on_waypoint_deleted(_selected_index);
                }

                ImGui::PushItemWidth(-1);
                std::string notes = to_utf8(waypoint.notes());
                if (ImGui::InputTextMultiline("Notes##notes", &notes))
                {
                    waypoint.set_notes(to_utf16(notes));
                    _route->set_unsaved(true);
                }
                ImGui::PopItemWidth();

                if (_randomizer_enabled)
                {
                    ImGui::Text("Randomizer");
                    load_randomiser_settings(waypoint);
                }
            }
        }
        ImGui::End();
    }

    void RouteWindow::render(bool vsync)
    {
        if (!render_host())
        {
            on_window_closed();
            return;
        }

        render_waypoint_list();
        render_waypoint_details();
    }

    void RouteWindow::set_route(IRoute* route) 
    {
        _route = route;
        _selected_index = 0u;
    }

    void RouteWindow::select_waypoint(uint32_t index)
    {
        _selected_index = index;
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

        if (ImGui::BeginTable("flags", 2))
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
                        if (ImGui::InputText(b.second.display.c_str(), &text))
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
                                    settings[b.first] = text;
                                    waypoint.set_randomizer_settings(settings);
                                    _route->set_unsaved(true);
                                }
                            }
                        }
                    }
                    break;
                }
                case RandomizerSettings::Setting::Type::Number:
                {
                    auto number = std::get<float>(value);
                    if (ImGui::InputFloat(b.second.display.c_str(), &number))
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
