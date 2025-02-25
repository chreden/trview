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
        calculate_column_widths();
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
        calculate_column_widths();
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
            if (ImGui::BeginTable(Names::camera_sink_list.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, -counter.height())))
            {
                imgui_header_row(
                    {
                        { "#", _column_sizer.size(0) },
                        { "Room", _column_sizer.size(1) },
                        { "Type", _column_sizer.size(2) },
                        {.name = "Hide", .width = _column_sizer.size(3), .set_checked = [&](bool v)
                            {
                                std::ranges::for_each(filtered_camera_sinks, [=](auto&& cs) { cs->set_visible(!v); });
                                on_scene_changed();
                            }, .checked = std::ranges::all_of(filtered_camera_sinks, [](auto&& cs) { return !cs->visible(); })
                        }
                    });

                imgui_sort_weak(_all_camera_sinks,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(primary_room(l), l.number()) < std::tuple(primary_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& camera_sink : filtered_camera_sinks)
                {
                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_camera_sink.lock() && _selected_camera_sink.lock()->number() == camera_sink->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to)
                    {
                        scroller.scroll_to_item();
                        _scroll_to = false;
                    }

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}##{0}", camera_sink->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav) | ImGuiTableFlags_SizingFixedFit))
                    {
                        scroller.fix_scroll();

                        set_local_selected_camera_sink(camera_sink);
                        if (_sync)
                        {
                            on_camera_sink_selected(camera_sink);
                        }

                        _scroll_to = false;
                    }

                    ImGui::TableNextColumn();
                    if (camera_sink->type() == ICameraSink::Type::Camera)
                    {
                        ImGui::Text(std::format("{}", primary_room(*camera_sink)).c_str());
                    }
                    else
                    {
                        ImGui::Text(inferred_rooms_text(camera_sink->inferred_rooms()).c_str());
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(camera_sink->type()).c_str());

                    ImGui::TableNextColumn();

                    bool hidden = !camera_sink->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", camera_sink->number()).c_str(), &hidden))
                    {
                        camera_sink->set_visible(!hidden);
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
                    ImGui::TableSetupScrollFreeze(1, 1);
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
        _filters.add_getter<float>("#", [](auto&& camera_sink) { return static_cast<float>(camera_sink.number()); });
        _filters.add_getter<float>("X", [](auto&& camera_sink) { return camera_sink.position().x * trlevel::Scale_X; });
        _filters.add_getter<float>("Y", [](auto&& camera_sink) { return camera_sink.position().y * trlevel::Scale_Y; });
        _filters.add_getter<float>("Z", [](auto&& camera_sink) { return camera_sink.position().z * trlevel::Scale_Z; });
        _filters.add_multi_getter<float>("Room", [](auto&& camera_sink) -> std::vector<float>
            {
                if (camera_sink.type() == ICameraSink::Type::Camera)
                {
                    return { static_cast<float>(primary_room(camera_sink)) };
                }
                return std::views::transform(
                    camera_sink.inferred_rooms(),
                    [](const auto& r) { if (auto room = r.lock()) { return static_cast<float>(room->number()); } return 0.0f; }) |
                    std::ranges::to<std::vector>();
            });
        _filters.add_multi_getter<float>("Trigger References", [&](auto&& camera_sink)
            {
                std::vector<float> results;
                for (const auto& trigger : camera_sink.triggers())
                {
                    const auto trigger_ptr = trigger.lock();
                    results.push_back(static_cast<float>(trigger_ptr->number()));
                }
                return results;
            });
        
        // Camera:
        _filters.add_getter<float>("Flag", [](auto&& camera_sink) { return static_cast<float>(camera_sink.flag()); }, is_camera);
        _filters.add_getter<bool>("Persistent", [](auto&& camera_sink) { return (camera_sink.flag() & 0x1) == 1; }, is_camera);
        
        // Sink:
        _filters.add_getter<float>("Strength", [](auto&& camera_sink) { return static_cast<float>(camera_sink.strength()); }, is_sink);
        _filters.add_getter<float>("Box Index", [](auto&& camera_sink) { return static_cast<float>(camera_sink.flag()); }, is_sink);
    }

    void CameraSinkWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("Type__", 2);
        _column_sizer.measure("Hide____", 3);

        for (const auto& camera_sink : _all_camera_sinks)
        {
            if (const auto camera_sink_ptr = camera_sink.lock())
            {
                _column_sizer.measure(std::to_string(camera_sink_ptr->number()), 0);
                if (camera_sink_ptr->type() == ICameraSink::Type::Camera)
                {
                    _column_sizer.measure(std::to_string(primary_room(*camera_sink_ptr)), 1);
                }
                else
                {
                    _column_sizer.measure(inferred_rooms_text(camera_sink_ptr->inferred_rooms()), 1);
                }
                _column_sizer.measure(to_string(camera_sink_ptr->type()), 2);
            }
        }
    }
}
