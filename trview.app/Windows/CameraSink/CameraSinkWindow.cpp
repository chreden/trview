#include "CameraSinkWindow.h"
#include "../../trview_imgui.h"
#include "../../Elements/IRoom.h"
#include "../RowCounter.h"
#include "../../Elements/Flyby/IFlybyNode.h"

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

        template <typename T>
        std::optional<T> get(
            const std::shared_ptr<IFlybyNode>& value, 
            const std::function<T (const IFlybyNode&)>& callback)
        {
            if (value)
            {
                return callback(*value);
            }
            return std::nullopt;
        };

        std::string flag_name(trlevel::PlatformAndVersion version, int index)
        {
            using namespace trlevel;
            switch (index)
            {
            case 0:
                return "Cut to Flyby";
            case 1:
                return version.version == LevelVersion::Tomb4 ? "Track Entity" : "Vignette";
            case 2:
                return "Loop";
            case 3:
                return "Focus Lara";
            case 4:
                return version.version == LevelVersion::Tomb4 ? "Lara's Last Head Pos" : "Hide Lara";
            case 5:
                return "Focus Lara's Head";
            case 6:
                return "Pan to Lara";
            case 7:
                return "Cut to Camera";
            case 8:
                return "Pause Movement";
            case 9:
                return "Disable Look";
            case 10:
                return "Disable Controls/Widscreen On";
            case 11:
                return "Enable Controls";
            case 12:
                return "Fade In";
            case 13:
                return "Fade Out";
            case 14:
                return "Heavy Triggers";
            case 15:
                return "One-Shot";
            }
            return "Unknown";
        }
    }

    ICameraSinkWindow::~ICameraSinkWindow()
    {
    }

    CameraSinkWindow::CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<ICamera>& camera)
        : _clipboard(clipboard), _camera(camera)
    {
        setup_filters();
        setup_flyby_filters();
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
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("Camera/Sink"))
                {
                    render_list();
                    ImGui::SameLine();
                    render_details();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Flyby"))
                {
                    render_flyby_list();
                    ImGui::SameLine();
                    render_flyby_tab();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
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
                }, default_hide(filtered_camera_sinks));
        }
        ImGui::EndChild();
    }

    void CameraSinkWindow::render_details()
    {
        auto selected = _selected_camera_sink.lock();
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true) && selected)
        {
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

    void CameraSinkWindow::set_flybys(const std::vector<std::weak_ptr<IFlyby>>& flybys)
    {
        _all_flybys = flybys;
        _state = {};
        if (!_all_flybys.empty()) { _selected_flyby = _all_flybys[0]; };
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

    void CameraSinkWindow::setup_flyby_filters()
    {
        _flyby_filters.set_columns(std::vector<std::string>{ "#", "Hide" });
        _flyby_filters.add_getter<int>("#", [](auto&& flyby) { return static_cast<int>(flyby.number()); });
        _flyby_filters.add_getter<bool>("Hide", [](auto&& flyby) { return !flyby.visible(); }, EditMode::ReadWrite);
        _flyby_filters.add_multi_getter<int>("Room", [](auto&& flyby)
            {
                std::unordered_set<int> rooms;
                for (const auto& node : flyby.nodes())
                {
                    if (const auto node_ptr = node.lock())
                    {
                        rooms.insert(node_ptr->room());
                    }
                }
                return rooms | std::ranges::to<std::vector>();
            });
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

    void CameraSinkWindow::render_flybys()
    {
        const auto selected_flyby = _selected_flyby.lock();
        if (!selected_flyby)
        {
            return;
        }

        if (ImGui::Button(_playing_flyby ? "X" : ">"))
        {
            if (!_playing_flyby)
            {
                if (_state.state == IFlyby::CameraState::State::Ended)
                {
                    _state = {};
                }
                else
                {
                    _state.state = IFlyby::CameraState::State::Active;
                }
            }
            _playing_flyby = !_playing_flyby;
        }
        ImGui::SameLine();

        const float offset_y = 6;
        const float offset_x = 4;

        const auto pos = ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2(offset_x, offset_y);
        const auto width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 20 - offset_x * 2;

        auto list = ImGui::GetWindowDrawList();

        const auto nodes = selected_flyby->nodes() | std::views::transform([](auto&& n) { return n.lock(); }) | std::ranges::to<std::vector>();
        if (!nodes.empty())
        {
            float current_node_x = 0;
            float next_node_x = 0;

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 75);
            if (ImGui::BeginTable("Flyby Data", 4, ImGuiTableFlags_SizingStretchSame))
            {
                auto add_row = [&](auto&& label, auto&& previous, auto&& current, auto&& next)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text(label);
                    ImGui::TableNextColumn();
                    current_node_x = ImGui::GetCursorPosX();
                    ImGui::Text(std::format("{}", previous).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::format("{}", current).c_str());
                    if (next)
                    {
                        ImGui::TableNextColumn();
                        next_node_x = ImGui::GetCursorPosX();
                        ImGui::Text(std::format("{}", next.value()).c_str());
                    }
                };

                auto add_flag_row = [&](auto&& label, auto&& previous, auto&& current, auto&& next)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text(label);
                        ImGui::TableNextColumn();
                        current_node_x = ImGui::GetCursorPosX();
                        ImGui::BeginDisabled();
                        ImGui::Checkbox(std::format("##{}_prev", label).c_str(), &previous);
                        ImGui::EndDisabled();
                        ImGui::TableNextColumn();
                        ImGui::BeginDisabled();
                        ImGui::Checkbox(std::format("##{}_current", label).c_str(), &current);
                        ImGui::EndDisabled();
                        if (next)
                        {
                            ImGui::TableNextColumn();
                            next_node_x = ImGui::GetCursorPosX();
                            ImGui::BeginDisabled();
                            ImGui::Checkbox(std::format("##{}_next", label).c_str(), &next.value());
                            ImGui::EndDisabled();
                        }
                    };

                std::shared_ptr<IFlybyNode> next;
                if (_state.index + 1 < nodes.size())
                {
                    next = nodes[_state.index + 1];
                }

                add_row("X", nodes[_state.index]->position().x * trlevel::Scale, _state.position.x * trlevel::Scale, get<int32_t>(next, [](auto&& n) { return static_cast<int32_t>(n.position().x * trlevel::Scale); }));
                add_row("Y", nodes[_state.index]->position().y * trlevel::Scale, _state.position.y * trlevel::Scale, get<int32_t>(next, [](auto&& n) { return static_cast<int32_t>(n.position().y * trlevel::Scale); }));
                add_row("Z", nodes[_state.index]->position().z * trlevel::Scale, _state.position.z * trlevel::Scale, get<int32_t>(next, [](auto&& n) { return static_cast<int32_t>(n.position().z * trlevel::Scale); }));
                add_row("DX", nodes[_state.index]->direction().x * trlevel::Scale, _state.raw_direction.x * trlevel::Scale, get<int32_t>(next, [](auto&& n) { return static_cast<int32_t>(n.direction().x * trlevel::Scale); }));
                add_row("DY", nodes[_state.index]->direction().y * trlevel::Scale, _state.raw_direction.y * trlevel::Scale, get<int32_t>(next, [](auto&& n) { return static_cast<int32_t>(n.direction().y * trlevel::Scale); }));
                add_row("DZ", nodes[_state.index]->direction().z * trlevel::Scale, _state.raw_direction.z * trlevel::Scale, get<int32_t>(next, [](auto&& n) { return static_cast<int32_t>(n.direction().z * trlevel::Scale); }));
                add_row("Roll", nodes[_state.index]->roll(), static_cast<int16_t>(_state.roll * -182.0f), get<int16_t>(next, [](auto&& n) { return n.roll(); }));
                add_row("Speed", nodes[_state.index]->speed(), _state.speed, get<uint16_t>(next, [](auto&& n) { return n.speed(); }));
                add_row("FOV", nodes[_state.index]->fov(), static_cast<int16_t>(_state.fov * 182.0f), get<int16_t>(next, [](auto&& n) { return n.fov(); }));
                add_row("Timer", nodes[_state.index]->timer(), _state.timer, get<uint16_t>(next, [](auto&& n) { return n.timer(); }));
                add_row("Room", nodes[_state.index]->room(), _state.room_id, get<uint32_t>(next, [](auto&& n) { return n.room(); }));

                // TODO: Flags
                for (int i = 0; i < 16; ++i)
                {
                    add_flag_row(
                        flag_name(_platform_and_version, i).c_str(),
                        (nodes[_state.index]->flags() & (1 << i)) != 0,
                        _state.flags[i] != 0,
                        get<bool>(next, [=](auto&& n) { return (n.flags() & (1 << i)) != 0; }));
                }

                ImGui::EndTable();
            }

            list->AddRectFilled(pos, pos + ImVec2(width, 5), ImColor(0.4f, 0.4f, 0.4f));

            // TODO: Handle 1 length flybys.
            const float spacing = 1.0f / (nodes.size() - 1);
            for (uint32_t i = 0; i < nodes.size(); ++i)
            {
                const float x = i * spacing * width;
                if (static_cast<int32_t>(i) == _state.index)
                {
                    const ImVec2 points[4] = {
                        pos + ImVec2(x, 0),
                        pos + ImVec2(x, 30),
                        pos + ImVec2(current_node_x, 30),
                        pos + ImVec2(current_node_x, 40)
                    };
                    list->AddPolyline(points, 4, ImColor(0.0f, 1.0f, 0.0f), ImDrawFlags_None, 4.0f);
                }
                else if (static_cast<int32_t>(i) == (_state.index + 1))
                {
                    const ImVec2 points[4] = {
                        pos + ImVec2(x, 0),
                        pos + ImVec2(x, 20),
                        pos + ImVec2(next_node_x, 20),
                        pos + ImVec2(next_node_x, 40)
                    };
                    list->AddPolyline(points, 4, ImColor(1.0f, 1.0f, 0.0f), ImDrawFlags_None, 4.0f);
                }
                else
                {
                    const ImVec2 points[2] =
                    {
                        pos + ImVec2(x, 0),
                        pos + ImVec2(x, 10),
                    };
                    list->AddPolyline(points, 2, ImColor(0.0f, 0.0f, 1.0f), ImDrawFlags_None, 4.0f);
                }
            }
        }

        ImGui::SetCursorPos(pos - ImGui::GetWindowPos() - ImVec2(offset_x, offset_y));
        const float step = 1.0f / static_cast<float>(selected_flyby->nodes().size() - 1);
        float percentage = (_state.t * step) + step * _state.index;
        ImGui::PushItemWidth(width + offset_x * 2);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 3.0f);
        if (ImGui::SliderFloat("##Position", &percentage, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
        {
            _state.index = static_cast<int32_t>(floor(percentage / step));
            _state.t = (percentage - (step * _state.index)) / step;
            _state.state = IFlyby::CameraState::State::Active;
            _state = selected_flyby->update_state(_state, 0.0f);
            sync_flyby();
        }
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();
    }

    void CameraSinkWindow::sync_flyby()
    {
        auto flyby = _selected_flyby.lock();
        if (!flyby)
        {
            return;
        }

        if (auto camera = _camera.lock())
        {
            using namespace DirectX::SimpleMath;
            camera->set_mode(ICamera::Mode::Free);
            camera->set_position(_state.position);
            camera->set_forward(_state.direction);
            camera->set_rotation_roll(_state.roll);
            camera->set_fov(_state.fov);
        }
    }

    void CameraSinkWindow::update(float delta)
    {
        auto flyby = _selected_flyby.lock();
        if (!flyby)
        {
            return;
        }

        if (_playing_flyby)
        {
            _state = flyby->update_state(_state, delta);
            if (_state.state == IFlyby::CameraState::State::Ended)
            {
                _playing_flyby = false;
                _state = {};
            }
            else
            {
                sync_flyby();
            }
        }
    }

    void CameraSinkWindow::set_platform_and_version(const trlevel::PlatformAndVersion& version)
    {
        _platform_and_version = version;
    }

    void CameraSinkWindow::render_flyby_list()
    {
        if (ImGui::BeginChild("##flybylist", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _flyby_filters.render();
            ImGui::SameLine();

            _flyby_filters.render_settings();

            auto filtered_flybys =
                _all_flybys |
                std::views::filter([&](auto&& flyby)
                    {
                        const auto flyby_ptr = flyby.lock();
                        return !(!flyby_ptr || !_flyby_filters.match(*flyby_ptr));
                    }) |
                std::views::transform([](auto&& flyby) { return flyby.lock(); }) | std::ranges::to<std::vector>();

            RowCounter counter{ "flyby", _all_flybys.size() };
            _flyby_filters.render_table(filtered_flybys, _all_flybys, _selected_flyby, counter,
                [&](auto&& flyby)
                {
                    _selected_flyby = flyby;
                    _state = {};
                }, default_hide(filtered_flybys));

            ImGui::EndChild();
        }
    }

    void CameraSinkWindow::render_flyby_tab()
    {
        auto selected = _selected_flyby.lock();
        if (ImGui::BeginChild("Flyby Details", ImVec2(), true) && selected)
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("Playback"))
                {
                    render_flybys();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Details"))
                {
                    render_flyby_details();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }

    void CameraSinkWindow::render_flyby_details()
    {
        if (auto selected_flyby = _selected_flyby.lock())
        {
            ImGui::Text("Nodes");
            if (ImGui::BeginTable("Nodes", 1))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableHeadersRow();

                const auto nodes = selected_flyby->nodes();
                uint32_t index = 0;
                for (const auto& node : nodes)
                {
                    node;

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_node.value_or(0) == index;
                    if (ImGui::Selectable(std::to_string(index).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        _selected_node = index;
                    }
                    ++index;
                }

                ImGui::EndTable();
            }

            if (_selected_node.has_value())
            {
                const auto nodes = selected_flyby->nodes();
                if (_selected_node.value() < nodes.size())
                {
                    if (const auto node = selected_flyby->nodes()[_selected_node.value()].lock())
                    {
                        const auto pos = node->position() * trlevel::Scale;
                        ImGui::Text(std::format("{},{},{}", static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y), static_cast<int32_t>(pos.z)).c_str());
                    }
                }
            }
        }
    }
}
