#include "CameraSinkWindow.h"
#include "../../trview_imgui.h"

namespace trview
{
    namespace
    {
        bool matching_room(ICameraSink& camera_sink, uint32_t room)
        {
            if (camera_sink.type() == ICameraSink::Type::Camera)
            {
                return camera_sink.room() == room;
            }
            return std::ranges::contains(camera_sink.inferred_rooms(), static_cast<uint16_t>(room));
        }

        uint32_t primary_room(const ICameraSink& camera_sink)
        {
            if (camera_sink.type() == ICameraSink::Type::Camera)
            {
                return camera_sink.room();
            }
            const auto inferred_rooms = camera_sink.inferred_rooms();
            return inferred_rooms.empty() ? 0u : std::ranges::min(camera_sink.inferred_rooms());
        }

        bool is_camera(const ICameraSink& camera_sink)
        {
            return camera_sink.type() == ICameraSink::Type::Camera;
        }

        bool is_sink(const ICameraSink& camera_sink)
        {
            return camera_sink.type() == ICameraSink::Type::Sink;
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
        if (ImGui::BeginChild(Names::list_panel.c_str(), ImVec2(230, 0), true))
        {
            _filters.render();
            ImGui::SameLine();

            _track.render();
            ImGui::SameLine();
            bool sync = _sync;
            if (ImGui::Checkbox(Names::sync.c_str(), &sync))
            {
                set_sync(sync);
            }

            if (ImGui::BeginTable(Names::camera_sink_list.c_str(), 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort_weak(_all_camera_sinks,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(primary_room(l), l.number()) < std::tuple(primary_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& camera_sink_ptr : _all_camera_sinks)
                {
                    const auto camera_sink = camera_sink_ptr.lock();

                    if (_track.enabled<Type::Room>() && !matching_room(*camera_sink, _current_room) || !_filters.match(*camera_sink))
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_camera_sink.lock() && _selected_camera_sink.lock()->number() == camera_sink->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to)
                    {
                        scroller.scroll_to_item();
                        _scroll_to = false;
                    }

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

                    ImGui::SetItemAllowOverlap();

                    ImGui::TableNextColumn();
                    if (camera_sink->type() == ICameraSink::Type::Camera)
                    {
                        ImGui::Text(std::format("{}", camera_sink->room()).c_str());
                    }
                    else
                    {
                        auto rooms = camera_sink->inferred_rooms();
                        std::stringstream stream;
                        if (!rooms.empty())
                        {
                            stream << rooms[0];
                        }
                        for (std::size_t i = 1; i < rooms.size(); ++i)
                        {
                            stream << "," << rooms[i];
                        }
                        ImGui::Text(stream.str().c_str());
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(camera_sink->type()).c_str());

                    ImGui::TableNextColumn();

                    bool hidden = !camera_sink->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", camera_sink->number()).c_str(), &hidden))
                    {
                        on_camera_sink_visibility(camera_sink, !hidden);
                    }
                    ImGui::PopStyleVar();
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    void CameraSinkWindow::render_details()
    {
        auto selected = _selected_camera_sink.lock();
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(230, 0), true) && selected)
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
                    on_camera_sink_type_changed();
                }
                bool sink_selected = selected->type() == ICameraSink::Type::Sink;
                if (ImGui::Selectable("Sink##type", &sink_selected))
                {
                    selected->set_type(ICameraSink::Type::Sink);
                    on_camera_sink_type_changed();
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
                    add_stat("Room", selected->room());
                    add_stat("Persistent", selected->persistent());
                }
                else
                {
                    add_stat("Strength", selected->strength());
                    add_stat("Box Index", selected->box_index());
                    std::string inferred_rooms;
                    auto rooms = selected->inferred_rooms();
                    for (auto i = 0u; i < rooms.size(); ++i)
                    {
                        inferred_rooms += std::to_string(rooms[i]);
                        if (i != rooms.size() - 1)
                        {
                            inferred_rooms += ",";
                        }
                    }
                    add_stat("Inferred Room", inferred_rooms.c_str());
                }

                ImGui::EndTable();

                ImGui::Spacing();

                ImGui::Text("Triggered By");
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
                            [](auto&& l, auto&& r) { return std::tuple(l.room(), l.number()) < std::tuple(r.room(), r.number()); },
                            [](auto&& l, auto&& r) { return std::tuple(trigger_type_name(l.type()), l.number()) < std::tuple(trigger_type_name(r.type()), r.number()); },
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
                        ImGui::Text(std::to_string(trigger_ptr->room()).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(trigger_type_name(trigger_ptr->type()).c_str());
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

    void CameraSinkWindow::set_current_room(uint32_t room)
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
                    return { static_cast<float>(camera_sink.room()) };
                }
                return std::views::transform(
                    camera_sink.inferred_rooms(),
                    [](const auto& r) { return static_cast<float>(r); }) |
                    std::ranges::to<std::vector>();
            });
        _filters.add_multi_getter<float>("Triggered By", [&](auto&& camera_sink)
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
        _filters.add_getter<float>("Strength", [](auto&& camera_sink) { return static_cast<float>(camera_sink.room()); }, is_sink);
        _filters.add_getter<float>("Box Index", [](auto&& camera_sink) { return static_cast<float>(camera_sink.flag()); }, is_sink);
    }
}
