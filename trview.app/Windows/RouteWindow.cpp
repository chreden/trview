#include "RouteWindow.h"
#include <trview.app/Routing/Route.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/IClipboard.h>
#include "../trview_imgui.h"
#include <format>
#include "../Routing/IRandomizerRoute.h"
#include "../Elements/ILevel.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    const std::string RouteWindow::Names::colour = "colour";

    using namespace graphics;

    namespace
    {
        std::string trimmed_level_name(const std::string& input)
        {
            return input.substr(input.find_last_of("/\\") + 1);
        }
    }

    IRouteWindow::~IRouteWindow()
    {
    }

    RouteWindow::RouteWindow(const std::shared_ptr<IClipboard>& clipboard, const std::shared_ptr<IDialogs>& dialogs,
        const std::shared_ptr<IFiles>& files)
        : _clipboard(clipboard), _dialogs(dialogs), _files(files)
    {
    }

    void RouteWindow::render_waypoint_list()
    {
        if (_need_focus)
        {
            ImGui::SetNextWindowFocus();
            _need_focus = false;
        }

        const auto route = _route.lock();
        auto rando_route = std::dynamic_pointer_cast<IRandomizerRoute>(route);

        if (ImGui::BeginChild(Names::waypoint_list_panel.c_str(), ImVec2(rando_route ? 250 : 150.0f, 0), true))
        {
            if (rando_route)
            {
                if (ImGui::BeginTable("##levellist", 1, ImGuiTableFlags_ScrollY, ImVec2(100, -1)))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupScrollFreeze(1, 1);
                    ImGui::TableHeadersRow();

                    std::string selected_level;
                    if (auto level = rando_route->level().lock())
                    {
                        selected_level = trimmed_level_name(level->filename());
                    }

                    for (const auto& file : rando_route->filenames())
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = file == selected_level;
                        if (ImGui::Selectable(file.c_str(), &selected))
                        {
                            on_level_switch(file);
                        }
                    }

                    ImGui::EndTable();
                }
                ImGui::SameLine();
            }

            std::optional<uint32_t> move_from;
            std::optional<uint32_t> move_to;

            if (ImGui::BeginTable("##waypointslist", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (route) 
                {
                    for (uint32_t i = 0; i < route->waypoints(); ++i)
                    {
                        if (auto waypoint = route->waypoint(i).lock())
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            bool selected = _selected_index == i;

                            ImGuiScroller scroller;
                            if (selected && _scroll_to_waypoint)
                            {
                                scroller.scroll_to_item();
                                _scroll_to_waypoint = false;
                            }

                            if (ImGui::Selectable(std::to_string(i).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                            {
                                scroller.fix_scroll();

                                _selected_index = i;
                                on_waypoint_selected(i);

                                _scroll_to_waypoint = false;
                            }

                            ImGuiDragDropFlags src_flags = 0;
                            src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
                            src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
                            src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip;
                            if (ImGui::BeginDragDropSource(src_flags))
                            {
                                ImGui::SetDragDropPayload("RouteWindowWaypoint", &i, sizeof(int));
                                ImGui::EndDragDropSource();
                            }

                            if (ImGui::BeginDragDropTarget())
                            {
                                ImGuiDragDropFlags target_flags = 0;
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RouteWindowWaypoint", target_flags))
                                {
                                    move_from = *(const int*)payload->Data;
                                    move_to = i;
                                }
                                ImGui::EndDragDropTarget();
                            }

                            ImGui::TableNextColumn();
                            ImGui::Text(waypoint_text(*waypoint).c_str());
                        }
                    }
                }
                ImGui::EndTable();
            }

            if (move_from && move_to)
            {
                on_waypoint_reordered(move_from.value(), move_to.value());
                if (_selected_index == move_from.value())
                {
                    on_waypoint_selected(move_to.value());
                }
            }
        }
        ImGui::EndChild();
    }

    void RouteWindow::render_waypoint_details()
    {
        if (ImGui::BeginChild(Names::waypoint_details_panel.c_str(), ImVec2(), true))
        {
            auto route = _route.lock();
            if (route && _selected_index < route->waypoints())
            {
                if (auto waypoint = route->waypoint(_selected_index).lock())
                {
                    if (ImGui::BeginTable(Names::waypoint_stats.c_str(), 2, 0, ImVec2(-1, 80)))
                    {
                        ImGui::TableSetupColumn("Name");
                        ImGui::TableSetupColumn("Value");
                        ImGui::TableNextRow();

                        auto add_stat = [&]<typename T>(const std::string & name, const T && value)
                        {
                            const auto string_value = get_string(value);
                            ImGui::TableNextColumn();
                            if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                _clipboard->write(to_utf16(string_value));
                                _tooltip_timer = 0.0f;
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text(string_value.c_str());
                        };

                        auto get_room_pos = [&waypoint, this]()
                        {
                            if (waypoint->room() < _all_rooms.size())
                            {
                                const auto room = _all_rooms[waypoint->room()].lock();
                                if (!room)
                                {
                                    return waypoint->position();
                                }
                                const auto info = room->info();
                                const Vector3 bottom_left = Vector3(static_cast<float>(info.x), static_cast<float>(info.yBottom), static_cast<float>(info.z)) / trlevel::Scale_X;
                                return waypoint->position() - bottom_left;
                            }
                            return waypoint->position();
                        };

                        auto position_text = [](const auto& pos)
                        {
                            const auto p = pos * trlevel::Scale;
                            return std::format("{:.0f}, {:.0f}, {:.0f}", p.x, p.y, p.z);
                        };

                        add_stat("Type", waypoint_type_to_string(waypoint->type()));
                        add_stat("Room", waypoint->room());

                        add_stat("Room Position", position_text(get_room_pos()));
                        ImGui::EndTable();
                    }

                    const auto pos = waypoint->position();
                    int pos_value[3] = { static_cast<int>(pos.x * trlevel::Scale), static_cast<int>(pos.y * trlevel::Scale), static_cast<int>(pos.z * trlevel::Scale) };
                    if (ImGui::DragScalarN("Position", ImGuiDataType_S32, pos_value, 3))
                    {
                        waypoint->set_position(Vector3(static_cast<float>(pos_value[0]), static_cast<float>(pos_value[1]), static_cast<float>(pos_value[2])) / trlevel::Scale);
                        on_waypoint_changed();
                    }

                    if (ImGui::BeginPopupContextItem("Position"))
                    {
                        if (ImGui::MenuItem("Copy"))
                        {
                            _clipboard->write(to_utf16(std::format("{},{},{}", pos_value[0], pos_value[1], pos_value[2])));
                        }
                        ImGui::EndPopup();
                    }

                    if (!_randomizer_enabled)
                    {
                        const std::string save_text = waypoint->has_save() ? "SAVEGAME.0" : Names::attach_save.c_str();
                        if (ImGui::Button(save_text.c_str(), ImVec2(-24, 18)))
                        {
                            if (!waypoint->has_save())
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
                                            waypoint->set_save_file(bytes.value());
                                            route->set_unsaved(true);
                                        }
                                    }
                                    catch (...)
                                    {
                                        _dialogs->message_box(L"Failed to attach save", L"Error", IDialogs::Buttons::OK);
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
                                        _files->save_file(filename.value().filename, waypoint->save_file());
                                    }
                                    catch (...)
                                    {
                                        _dialogs->message_box(L"Failed to export save", L"Error", IDialogs::Buttons::OK);
                                    }
                                }
                            }
                        }
                        ImGui::SameLine();
                        if (ImGui::Button(Names::clear_save.c_str(), ImVec2(-1, 0)))
                        {
                            if (waypoint->has_save())
                            {
                                waypoint->set_save_file({});
                                route->set_unsaved(true);
                            }
                        }
                    }

                    if (ImGui::Button(Names::delete_waypoint.c_str(), ImVec2(-1, 0)))
                    {
                        on_waypoint_deleted(_selected_index);
                    }
                    else
                    {
                        // Don't access the waypoint after it has been deleted - this is an issue with the window not
                        // having temporary ownership of the waypoint - if it was shared_ptr it would be fine.
                        if (_randomizer_enabled)
                        {
                            ImGui::Text("Randomizer");
                            load_randomiser_settings(*waypoint);
                        }
                        else
                        {
                            ImGui::Text("Notes");
                            std::string notes = waypoint->notes();
                            if (ImGui::InputTextMultiline(Names::notes.c_str(), &notes, ImVec2(-1, -1)))
                            {
                                waypoint->set_notes(notes);
                                route->set_unsaved(true);
                            }
                        }
                    }
                }
            }
        }
        ImGui::EndChild();
    }

    bool RouteWindow::render_route_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(420, 500));

        std::string window_title;
        if (const auto route = _route.lock())
        {
            const auto filename = route->filename();
            if (filename)
            {
                window_title = std::format(" - {}", filename.value());
            }
        }

        if (ImGui::Begin(std::format("Route{}###Route", window_title).c_str(), &stay_open, ImGuiWindowFlags_MenuBar))
        {
            render_menu_bar();
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

    void RouteWindow::set_route(const std::weak_ptr<IRoute>& route) 
    {
        _route = route;
        _selected_index = 0u;
    }

    void RouteWindow::select_waypoint(uint32_t index)
    {
        _selected_index = index;
        _scroll_to_waypoint = true;
    }

    /// Set the items to that are in the level.
    /// @param items The items to show.
    void RouteWindow::set_items(const std::vector<std::weak_ptr<IItem>>& items)
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
        _need_focus = true;
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
        auto route = _route.lock();

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
                        waypoint_settings[b.first] = value;
                        waypoint.set_randomizer_settings(waypoint_settings);
                        route->set_unsaved(true);
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
                            route->set_unsaved(true);
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
                                    route->set_unsaved(true);
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
                        route->set_unsaved(true);
                    }
                    break;
                }
            }
        }
    }

    std::string RouteWindow::waypoint_text(const IWaypoint& waypoint) const
    {
        if (waypoint.type() == IWaypoint::Type::Entity)
        {
            if (waypoint.index() < _all_items.size())
            {
                auto item = _all_items[waypoint.index()].lock();
                if (item)
                {
                    return item->type();
                }
                return "Invalid entity";
            }
            else
            {
                return "Invalid entity";
            }
        }
        else if (waypoint.type() == IWaypoint::Type::Trigger)
        {
            if (waypoint.index() < _all_triggers.size())
            {
                if (auto trigger = _all_triggers[waypoint.index()].lock())
                {
                    return trigger_type_name(trigger->type());
                }
            }
            else
            {
                return "Invalid trigger";
            }
        }
        return waypoint_type_to_string(waypoint.type());
    }

    void RouteWindow::render_menu_bar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (_randomizer_enabled)
                {
                    if (ImGui::BeginMenu("New"))
                    {
                        if (ImGui::MenuItem("Route"))
                        {
                            on_new_route();
                        }

                        if (ImGui::MenuItem("Randomizer Route"))
                        {
                            on_new_randomizer_route();
                        }
                        ImGui::EndMenu();
                    }
                }
                else if(ImGui::MenuItem("New"))
                {
                    on_new_route();
                }

                if (ImGui::MenuItem("Open"))
                {
                    on_route_open();
                }

                const auto route = _route.lock();
                if (ImGui::MenuItem("Reload", nullptr, nullptr, route && route->filename().has_value()))
                {
                    on_route_reload();
                }

                if (ImGui::MenuItem("Save"))
                {
                    on_route_save();
                }

                if (ImGui::MenuItem("Save As"))
                {
                    on_route_save_as();
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Settings"))
            {
                if (!_show_settings)
                {
                    ImGui::OpenPopup("SettingsPopup");
                }
                _show_settings = !_show_settings;
            }

            if (_show_settings && ImGui::BeginPopup("SettingsPopup"))
            {
                auto route = _route.lock();
                auto colour = route ? route->colour() : Colour::Green;
                if (ImGui::ColorEdit3("Route##colour", &colour.r, ImGuiColorEditFlags_NoInputs))
                {
                    on_colour_changed(colour);
                }
                auto waypoint_colour = route ? route->waypoint_colour() : Colour::White;
                if (ImGui::ColorEdit3("Waypoint##colour", &waypoint_colour.r, ImGuiColorEditFlags_NoInputs))
                {
                    on_waypoint_colour_changed(waypoint_colour);
                }
                ImGui::EndPopup();
            }
            else
            {
                _show_settings = false;
            }

            ImGui::EndMenuBar();
        }
    }
}
