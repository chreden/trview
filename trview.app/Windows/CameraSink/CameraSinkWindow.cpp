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
    }

    ICameraSinkWindow::~ICameraSinkWindow()
    {
    }

    CameraSinkWindow::CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
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
    }

    bool CameraSinkWindow::render_camera_sink_window()
    {
        bool stay_open = true;
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_list();
            ImGui::SameLine();
            render_details();
        }
        ImGui::End();
        return stay_open;
    }

    void CameraSinkWindow::set_track_room(bool value)
    {
        _track_room = value;
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
    }

    void CameraSinkWindow::render_list()
    {
        if (ImGui::BeginChild(Names::list_panel.c_str(), ImVec2(230, 0), true))
        {
            _filters.render();
            ImGui::SameLine();

            bool track_room = _track_room;
            if (ImGui::Checkbox(Names::track_room.c_str(), &track_room))
            {
                set_track_room(track_room);
            }
            ImGui::SameLine();
            bool sync = _sync;
            if (ImGui::Checkbox(Names::sync.c_str(), &sync))
            {
                set_sync(sync);
            }

            if (ImGui::BeginTable(Names::camera_sink_list.c_str(), 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                for (const auto& camera_sink_ptr : _all_camera_sinks)
                {
                    const auto camera_sink = camera_sink_ptr.lock();

                    if (_track_room && !matching_room(*camera_sink, _current_room) || !_filters.match(*camera_sink))
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
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(230, 0), true))
        {
            auto selected = _selected_camera_sink.lock();
            if (selected && ImGui::BeginTabBar("TabBar"))
            {
                const auto add_stat = [&]<typename T>(const std::string & name, const T && value)
                {
                    const auto string_value = get_string(value);
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        _clipboard->write(to_utf16(string_value));
                        _tooltip_timer = 0.0f;
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

                if (ImGui::BeginCombo("Type", to_string(selected->type()).c_str()))
                {
                    bool camera_selected = selected->type() == ICameraSink::Type::Camera;
                    if (ImGui::Selectable("Camera##type", &camera_selected))
                    {
                        selected->set_type(ICameraSink::Type::Camera);
                    }
                    bool sink_selected = selected->type() == ICameraSink::Type::Sink;
                    if (ImGui::Selectable("Sink##type", &sink_selected))
                    {
                        selected->set_type(ICameraSink::Type::Sink);
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
                        add_stat("Persistent", selected->flag() & 0x1);
                    }
                    else
                    {
                        add_stat("Strength", selected->room());
                        add_stat("Box Index", selected->flag());
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
                }

                ImGui::EndTabBar();
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
}
