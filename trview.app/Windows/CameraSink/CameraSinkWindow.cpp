#include "CameraSinkWindow.h"
#include "../../trview_imgui.h"
#include "../../Elements/IRoom.h"
#include "../RowCounter.h"

namespace trview
{
    namespace
    {
        bool matching_room(ICameraSink& camera_sink, const std::weak_ptr<IRoom>& room)
        {
            const auto room_ptr = room.lock();
            if (camera_sink.type() == ICameraSink::Type::Camera)
            {
                return camera_sink.room().lock() == room_ptr;
            }

            const auto inferred = camera_sink.inferred_rooms();
            return std::ranges::find_if(inferred, [=](auto&& r) { return r.lock() == room_ptr; }) != inferred.end();
        }

        uint32_t primary_room(const ICameraSink& camera_sink)
        {
            if (camera_sink.type() == ICameraSink::Type::Camera)
            {
                if (auto room = camera_sink.room().lock())
                {
                    return room->number();
                }
                return 0u;
            }
            
            const auto inferred = camera_sink.inferred_rooms();
            if (inferred.empty())
            {
                return 0u;
            }
            const auto view = inferred | std::views::transform([](auto&& r) { if (auto room = r.lock()) { return room->number(); } return 0u; });
            return *std::ranges::min_element(view);
        }

        bool is_camera(const ICameraSink& camera_sink)
        {
            return camera_sink.type() == ICameraSink::Type::Camera;
        }

        bool is_sink(const ICameraSink& camera_sink)
        {
            return camera_sink.type() == ICameraSink::Type::Sink;
        }

        std::string inferred_rooms_text(const std::vector<std::weak_ptr<IRoom>>& rooms)
        {
            std::string inferred_rooms;
            for (auto i = 0u; i < rooms.size(); ++i)
            {
                if (const auto room = rooms[i].lock())
                {
                    inferred_rooms += std::to_string(room->number());
                    if (i != rooms.size() - 1)
                    {
                        inferred_rooms += ",";
                    }
                }
            }
            return inferred_rooms;
        }
    }

    ICameraSinkWindow::~ICameraSinkWindow()
    {
    }

    CameraSinkWindow::CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        setup_filters();

        _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                _settings.camera_sink_window_columns = _filters.columns();
                on_settings(_settings);
            };
    }

    void CameraSinkWindow::render()
    {
        if (!render_camera_sink_window())
        {
            on_window_closed();
            return;
        }
    }

    void CameraSinkWindow::set_number(int32_t number)
    {
        _id = std::format("Camera/Sink {}", number);
    }

    void CameraSinkWindow::set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks)
    {
        _all_camera_sinks = camera_sinks;
        _force_sort = true;
        _filters.force_sort();
    }

    bool CameraSinkWindow::render_camera_sink_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(485, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_list();
            ImGui::SameLine();
            render_details();
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void CameraSinkWindow::set_sync(bool value)
    {
        if (_sync != value)
        {
            _sync = value;
            _scroll_to = true;
            if (_sync && _global_selected_camera_sink.lock())
            {
                set_selected_camera_sink(_global_selected_camera_sink);
            }
        }
    }
    
    void CameraSinkWindow::set_local_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _selected_camera_sink = camera_sink;
        _triggered_by.clear();
        if (auto cs = camera_sink.lock())
        {
            _triggered_by = cs->triggers();
        }
    }

    void CameraSinkWindow::render_list()
    {
        if (ImGui::BeginChild(Names::list_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _auto_hider.check_focus();
            _filters.render();
            ImGui::SameLine();

            _track.render();
            ImGui::SameLine();
            bool sync = _sync;
            if (ImGui::Checkbox(Names::sync.c_str(), &sync))
            {
                set_sync(sync);
            }

            _auto_hider.render();
            ImGui::SameLine();
            _filters.render_settings();

            auto filtered_camera_sinks =
                _all_camera_sinks |
                std::views::filter([&](auto&& cs)
                    {
                        const auto cs_ptr = cs.lock();
                        return !(!cs_ptr || (_track.enabled<Type::Room>() && !matching_room(*cs_ptr, _current_room) || !_filters.match(*cs_ptr)));
                    }) |
                std::views::transform([](auto&& cs) { return cs.lock(); }) |
                std::ranges::to<std::vector>();

            if (_auto_hider.apply(_all_camera_sinks, filtered_camera_sinks, _filters))
            {
                on_scene_changed();
            }

            RowCounter counter{ "camera/sink", _all_camera_sinks.size() };
            _filters.render_table(filtered_camera_sinks, _all_camera_sinks, _selected_camera_sink, counter,
                [&](auto&& camera)
                {
                    set_local_selected_camera_sink(camera);
                    if (_sync)
                    {
                        on_camera_sink_selected(camera);
                    }
                },
                {
                    {
                        "Hide",
                        {
                            .on_toggle = [&](auto&& camera, auto&& value)
                                {
                                    if (auto camera_ptr = camera.lock())
                                    {
                                        camera_ptr->set_visible(!value);
                                        on_scene_changed();
                                    }
                                },
                            .on_toggle_all = [&](bool value)
                                {
                                    std::ranges::for_each(filtered_camera_sinks, [=](auto&& camera) { camera->set_visible(!value); });
                                    on_scene_changed();
                                },
                            .all_toggled = [&]() { return std::ranges::all_of(filtered_camera_sinks, [](auto&& camera) { return !camera->visible(); }); }
                        }
                    }
                });
        }
        ImGui::EndChild();
    }

    void CameraSinkWindow::render_details()
    {
        auto selected = _selected_camera_sink.lock();
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true) && selected)
        {
            const auto add_stat = [&]<typename T>(const std::string & name, const T && value)
            {
                const auto string_value = get_string(value);
                ImGui::TableNextColumn();
                ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav));
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Copy"))
                    {
                        _clipboard->write(to_utf16(string_value));
                    }
                    ImGui::EndPopup();
                }
                if (_tips.find(name) != _tips.end())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text(_tips[name].c_str());
                    ImGui::EndTooltip();
                }
                ImGui::TableNextColumn();
                ImGui::Text(string_value.c_str());
            };

            if (ImGui::BeginCombo(Names::type.c_str(), to_string(selected->type()).c_str()))
            {
                bool camera_selected = selected->type() == ICameraSink::Type::Camera;
                if (ImGui::Selectable("Camera##type", &camera_selected))
                {
                    selected->set_type(ICameraSink::Type::Camera);
                    on_scene_changed();
                }
                bool sink_selected = selected->type() == ICameraSink::Type::Sink;
                if (ImGui::Selectable("Sink##type", &sink_selected))
                {
                    selected->set_type(ICameraSink::Type::Sink);
                    on_scene_changed();
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginTable(Names::stats_listbox.c_str(), 2, 0, ImVec2(-1, 0)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                add_stat("#", selected->number());
                const auto pos = selected->position() * trlevel::Scale;
                add_stat("Position", std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z));

                if (selected->type() == ICameraSink::Type::Camera)
                {
                    add_stat("Flag", selected->flag());
                    add_stat("Room", primary_room(*selected));
                    add_stat("Persistent", selected->persistent());
                }
                else
                {
                    add_stat("Strength", selected->strength());
                    add_stat("Box Index", selected->box_index());
                    add_stat("Inferred Room", inferred_rooms_text(selected->inferred_rooms()));
                }

                ImGui::EndTable();

                ImGui::Spacing();

                ImGui::Text("Trigger References");
                if (ImGui::BeginTable(Names::triggers_list.c_str(), 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
                {
                    ImGui::TableSetupColumn("#");
                    ImGui::TableSetupColumn("Room");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableHeadersRow();

                    imgui_sort_weak(_triggered_by,
                        {
                            [](auto&& l, auto&& r) { return l.number() < r.number(); },
                            [](auto&& l, auto&& r) { return std::tuple(trigger_room(l), l.number()) < std::tuple(trigger_room(r), r.number()); },
                            [](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); },
                        }, _force_sort);

                    for (auto& trigger : _triggered_by)
                    {
                        auto trigger_ptr = trigger.lock();
                        if (!trigger_ptr)
                        {
                            continue;
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool trigger_selected = _selected_trigger.lock() == trigger_ptr;
                        if (ImGui::Selectable(std::to_string(trigger_ptr->number()).c_str(), &trigger_selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                        {
                            _selected_trigger = trigger;
                            _track.set_enabled<Type::Room>(false);
                            on_trigger_selected(trigger);
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(trigger_room(trigger_ptr)).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(to_string(trigger_ptr->type()).c_str());
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::EndChild();
    }

    void CameraSinkWindow::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _global_selected_camera_sink = camera_sink;
        if (_sync)
        {
            _scroll_to = true;
            set_local_selected_camera_sink(camera_sink);
        }
    }

    void CameraSinkWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
    }

    void CameraSinkWindow::setup_filters()
    {
        _filters.clear_all_getters();

        // All:
        std::set<std::string> available_types{ "Camera", "Sink" };
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& camera_sink) { return to_string(camera_sink.type()); });
        _filters.add_getter<int>("#", [](auto&& camera_sink) { return static_cast<int>(camera_sink.number()); });
        _filters.add_getter<int>("X", [](auto&& camera_sink) { return static_cast<int>(camera_sink.position().x * trlevel::Scale_X); });
        _filters.add_getter<int>("Y", [](auto&& camera_sink) { return static_cast<int>(camera_sink.position().y * trlevel::Scale_Y); });
        _filters.add_getter<int>("Z", [](auto&& camera_sink) { return static_cast<int>(camera_sink.position().z * trlevel::Scale_Z); });
        _filters.add_getter<int>("Room", [](auto&& camera_sink)
            {
                if (camera_sink.type() == ICameraSink::Type::Camera)
                {
                    return static_cast<int>(primary_room(camera_sink));
                }
                const auto rooms = std::views::transform(camera_sink.inferred_rooms(),
                    [](const auto& r) { if (auto room = r.lock()) { return static_cast<int>(room->number()); } return 0; }) |
                    std::ranges::to<std::vector>();
                return rooms.empty() ? 0 : rooms[0];
            });
        _filters.add_multi_getter<int>("Rooms", [](auto&& camera_sink) -> std::vector<int>
            {
                if (camera_sink.type() == ICameraSink::Type::Camera)
                {
                    return { static_cast<int>(primary_room(camera_sink)) };
                }
                return std::views::transform(
                    camera_sink.inferred_rooms(),
                    [](const auto& r) { if (auto room = r.lock()) { return static_cast<int>(room->number()); } return 0; }) |
                    std::ranges::to<std::vector>();
            });
        _filters.add_multi_getter<int>("Trigger References", [&](auto&& camera_sink)
            {
                std::vector<int> results;
                for (const auto& trigger : camera_sink.triggers())
                {
                    const auto trigger_ptr = trigger.lock();
                    results.push_back(static_cast<int>(trigger_ptr->number()));
                }
                return results;
            });
        _filters.add_getter<bool>("Hide", [](auto&& camera_sink) { return !camera_sink.visible(); }, EditMode::ReadWrite);

        // Camera:
        _filters.add_getter<int>("Flag", [](auto&& camera_sink) { return static_cast<int>(camera_sink.flag()); }, is_camera);
        _filters.add_getter<bool>("Persistent", [](auto&& camera_sink) { return (camera_sink.flag() & 0x1) == 1; }, is_camera);
        
        // Sink:
        _filters.add_getter<int>("Strength", [](auto&& camera_sink) { return static_cast<int>(camera_sink.strength()); }, is_sink);
        _filters.add_getter<int>("Box Index", [](auto&& camera_sink) { return static_cast<int>(camera_sink.flag()); }, is_sink);
    }

    void CameraSinkWindow::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        if (!_columns_set)
        {
            _filters.set_columns(settings.camera_sink_window_columns);
            _columns_set = true;
        }
    }
}
