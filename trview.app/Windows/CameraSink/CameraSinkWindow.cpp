#include "CameraSinkWindow.h"
#include "../../trview_imgui.h"
#include "../../Elements/IRoom.h"
#include "../RowCounter.h"
#include "../../Elements/Flyby/IFlybyNode.h"
#include "../../Messages/Messages.h"
#include "../../Elements/ILevel.h"
#include "../../Elements/ElementFilters.h"

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

    void add_camera_sink_filters(Filters& filters)
    {
        if (filters.has_type_key("ICameraSink"))
        {
            return;
        }

        std::set<std::string> available_types{ "Camera", "Sink" };
        auto camera_sink_getters = Filters::GettersBuilder()
            .with_type_key("ICameraSink")
            .with_getter<ICameraSink, std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& camera_sink) { return to_string(camera_sink.type()); })
            .with_getter<ICameraSink, int>("#", [](auto&& camera_sink) { return static_cast<int>(camera_sink.number()); })
            .with_getter<ICameraSink, int>("X", [](auto&& camera_sink) { return static_cast<int>(camera_sink.position().x * trlevel::Scale_X); })
            .with_getter<ICameraSink, int>("Y", [](auto&& camera_sink) { return static_cast<int>(camera_sink.position().y * trlevel::Scale_Y); })
            .with_getter<ICameraSink, int>("Z", [](auto&& camera_sink) { return static_cast<int>(camera_sink.position().z * trlevel::Scale_Z); })
            .with_getter<ICameraSink, std::weak_ptr<IFilterable>>("Room", {}, [](auto&& camera_sink) -> std::weak_ptr<IFilterable>
                { 
                    if (camera_sink.type() == ICameraSink::Type::Camera)
                    {
                        return camera_sink.room();
                    }
                    const auto rooms = camera_sink.inferred_rooms() | std::ranges::to<std::vector<std::weak_ptr<IFilterable>>>();
                    return !rooms.empty() ? rooms[0] : std::weak_ptr<IFilterable>{};
                }, {}, EditMode::Read, "IRoom")
            .with_getter<ICameraSink, int>("Room #", [](auto&& camera_sink)
                {
                    if (camera_sink.type() == ICameraSink::Type::Camera)
                    {
                        return static_cast<int>(primary_room(camera_sink));
                    }
                    const auto rooms = std::views::transform(camera_sink.inferred_rooms(),
                        [](const auto& r) { if (auto room = r.lock()) { return static_cast<int>(room->number()); } return 0; }) |
                        std::ranges::to<std::vector>();
                    return rooms.empty() ? 0 : rooms[0];
                })
            .with_multi_getter<ICameraSink, std::weak_ptr<IFilterable>>("Rooms", {}, [](auto&& camera_sink) -> std::vector<std::weak_ptr<IFilterable>>
                {
                    if (camera_sink.type() == ICameraSink::Type::Camera)
                    {
                        return { camera_sink.room() };
                    }
                    return camera_sink.inferred_rooms() | std::ranges::to<std::vector<std::weak_ptr<IFilterable>>>();
                }, {}, "IRoom")
            .with_multi_getter<ICameraSink, int>("Rooms #", [](auto&& camera_sink) -> std::vector<int>
                {
                    if (camera_sink.type() == ICameraSink::Type::Camera)
                    {
                        return { static_cast<int>(primary_room(camera_sink)) };
                    }
                    return std::views::transform(
                        camera_sink.inferred_rooms(),
                        [](const auto& r) { if (auto room = r.lock()) { return static_cast<int>(room->number()); } return 0; }) |
                        std::ranges::to<std::vector>();
                })
            .with_multi_getter<ICameraSink, std::weak_ptr<IFilterable>>("Trigger", {}, [](auto&& camera_sink) -> std::vector<std::weak_ptr<IFilterable>>
                {
                    return camera_sink.triggers() | std::ranges::to<std::vector<std::weak_ptr<IFilterable>>>();
                }, {}, "ITrigger")
            .with_multi_getter<ICameraSink, int>("Trigger References", [&](auto&& camera_sink)
                {
                    std::vector<int> results;
                    for (const auto& trigger : camera_sink.triggers())
                    {
                        const auto trigger_ptr = trigger.lock();
                        results.push_back(static_cast<int>(trigger_ptr->number()));
                    }
                    return results;
                })
            .with_getter<ICameraSink, bool>("Hide", [](auto&& camera_sink) { return !camera_sink.visible(); }, EditMode::ReadWrite)
            // Camera:
            .with_getter<ICameraSink, int>("Flag", [](auto&& camera_sink) { return static_cast<int>(camera_sink.flag()); }, is_camera)
            .with_getter<ICameraSink, bool>("Persistent", [](auto&& camera_sink) { return (camera_sink.flag() & 0x1) == 1; }, is_camera)

            // Sink:
            .with_getter<ICameraSink, int>("Strength", [](auto&& camera_sink) { return static_cast<int>(camera_sink.strength()); }, is_sink)
            .with_getter<ICameraSink, int>("Box Index", [](auto&& camera_sink) { return static_cast<int>(camera_sink.flag()); }, is_sink)
            .with_getter<ICameraSink, bool>("In Visible Room", [](auto&& camera_sink)
                {
                    if (const auto level = camera_sink.level().lock())
                    {
                        return level->is_in_visible_set(camera_sink.room());
                    }
                    return false;
                });

        filters.add_getters(camera_sink_getters.build<ICameraSink>());
    }

    void add_flyby_filters(Filters& filters)
    {
        if (filters.has_type_key("IFlyby"))
        {
            return;
        }

        auto flyby_getters = Filters::GettersBuilder()
            .with_type_key("IFlyby")
            .with_getter<IFlyby, int>("#", [](auto&& flyby) { return static_cast<int>(flyby.number()); })
            .with_getter<IFlyby, bool>("Hide", [](auto&& flyby) { return !flyby.visible(); }, EditMode::ReadWrite)
            .with_multi_getter<IFlyby, int>("Room", [](auto&& flyby)
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
            })
            .with_getter<IFlyby, bool>("In Visible Room", [](auto&& flyby)
            {
                if (const auto level = flyby.level().lock())
                {
                    for (const auto& node : flyby.nodes())
                    {
                        if (const auto node_ptr = node.lock())
                        {
                            if (level->is_in_visible_set(level->room(node_ptr->room())))
                            {
                                return true;
                            }
                        }
                    }
                }
                return false;
            });

        filters.add_getters(flyby_getters.build<IFlyby>());
    }

    void add_flyby_node_filters(Filters& filters, const std::weak_ptr<ILevel>& level)
    {
        if (filters.has_type_key("IFlybyNode"))
        {
            return;
        }

        const auto level_ptr = level.lock();
        const auto platform_and_version = level_ptr ? level_ptr->platform_and_version() : trlevel::PlatformAndVersion{ .platform = trlevel::Platform::PC, .version = trlevel::LevelVersion::Tomb4 };

        auto flyby_node_getters = Filters::GettersBuilder()
            .with_type_key("IFlybyNode")
            .with_getter<IFlybyNode, int>("#", [](auto&& node) { return static_cast<int>(node.number()); })
            .with_getter<IFlybyNode, int>("X", [](auto&& node) { return static_cast<int>(node.position().x * trlevel::Scale_X); })
            .with_getter<IFlybyNode, int>("Y", [](auto&& node) { return static_cast<int>(node.position().y * trlevel::Scale_Y); })
            .with_getter<IFlybyNode, int>("Z", [](auto&& node) { return static_cast<int>(node.position().z * trlevel::Scale_Z); })
            .with_getter<IFlybyNode, int>("Room", [](auto&& node) { return node.room(); })
            .with_getter<IFlybyNode, int>("Roll", [](auto&& node) { return node.roll(); })
            .with_getter<IFlybyNode, int>("Speed", [](auto&& node) { return node.speed(); })
            .with_getter<IFlybyNode, int>("Fov", [](auto&& node) { return node.fov(); })
            .with_getter<IFlybyNode, int>("Timer", [](auto&& node) { return node.timer(); });
            for (int i = 0; i < 16; ++i)
            {
                flyby_node_getters.with_getter<IFlybyNode, bool>(flag_name(platform_and_version, i), [=](auto&& node) { return (node.flags() & (1 << i)) != 0; });
            }

        filters.add_getters(flyby_node_getters.build<IFlybyNode>());
    }

    CameraSinkWindow::CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<ICamera>& camera, const std::weak_ptr<IMessageSystem>& messaging)
        : _clipboard(clipboard), _camera(camera), _messaging(messaging)
    {
        setup_filters();
        setup_flyby_filters();
    }

    void CameraSinkWindow::render()
    {
        if (!_settings)
        {
            messages::get_settings(_messaging, weak_from_this());
        }

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
        setup_filters();
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

                if (ImGui::BeginTabItem("Flyby", 0, _go_to_flybys ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None))
                {
                    _go_to_flybys = false;
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
            _filters.scroll_to_item();
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

    void CameraSinkWindow::set_local_selected_flyby(const std::weak_ptr<IFlyby>& flyby)
    {
        _selected_flyby = flyby;
        if (auto selected_flyby = _selected_flyby.lock())
        {
            const auto nodes = selected_flyby->nodes();
            if (nodes.empty())
            {
                set_local_selected_flyby_node({});
            }
            else
            {
                set_local_selected_flyby_node(nodes[0]);
                if (_sync)
                {
                    messages::send_select_flyby_node(_messaging, _selected_node);
                }
            }
        }
        else
        {
            set_local_selected_flyby_node({});
        }
        _state = {};
        restore_camera_state();
    }

    void CameraSinkWindow::set_local_selected_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node)
    {
        if (auto selected_node = flyby_node.lock())
        {
            const auto current = _selected_flyby.lock();
            if (current != selected_node->flyby().lock())
            {
                set_local_selected_flyby(selected_node->flyby());
                _go_to_flybys = true;
            }
        }
        _selected_node = flyby_node;
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

            _auto_hider.apply(_all_camera_sinks, filtered_camera_sinks, _filters.test_and_reset_changed());

            RowCounter counter{ "camera/sink", _all_camera_sinks.size() };
            _filters.render_table(filtered_camera_sinks, _all_camera_sinks, _selected_camera_sink, counter,
                [&](auto&& camera)
                {
                    const std::shared_ptr<ICameraSink> f_ptr = std::static_pointer_cast<ICameraSink>(camera.lock());
                    set_local_selected_camera_sink(f_ptr);
                    if (_sync)
                    {
                        messages::send_select_camera_sink(_messaging, f_ptr);
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
                            messages::send_select_trigger(_messaging, trigger);
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
    }

    void CameraSinkWindow::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _global_selected_camera_sink = camera_sink;
        if (_sync)
        {
            _filters.scroll_to_item();
            set_local_selected_camera_sink(camera_sink);
        }
    }

    void CameraSinkWindow::set_selected_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node)
    {
        _global_selected_flyby_node = flyby_node;
        if (_sync)
        {
            set_local_selected_flyby_node(flyby_node);
        }
    }

    void CameraSinkWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
    }

    void CameraSinkWindow::setup_filters()
    {
        _filters.clear_all_getters();
        add_all_filters(_filters, _level);

        _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->camera_sink_window_columns = _filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };
        _filters.set_type_key("ICameraSink");
    }

    void CameraSinkWindow::setup_flyby_filters()
    {
        _flyby_filters.clear_all_getters();
        add_all_filters(_flyby_filters, _level);
        _flyby_filters.set_type_key("IFlyby");

        _node_filters.clear_all_getters();
        add_all_filters(_node_filters, _level);
        _node_filters.set_type_key("IFlybyNode");

        _flyby_filters.set_columns(std::vector<std::string>{ "#", "Hide" });
        _token_store += _flyby_filters.on_columns_reset += [this]()
            {
                _flyby_filters.set_columns(std::vector<std::string>{ "#", "Hide" });
            };
        _token_store += _flyby_filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->flyby_columns = _flyby_filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };

        _node_filters.set_columns(std::vector<std::string>{ "#", "Room" });
        _token_store += _node_filters.on_columns_reset += [this]()
            {
                _node_filters.set_columns(std::vector<std::string>{ "#", "Room" });
            };
        _token_store += _node_filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->flyby_node_columns = _node_filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };
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
                capture_camera_state();
                if (_state.state == IFlyby::CameraState::State::Ended)
                {
                    _state = {};
                }
                else
                {
                    _state.state = IFlyby::CameraState::State::Active;
                }
            }
            else
            {
                restore_camera_state();
            }
            _playing_flyby = !_playing_flyby;
        }
        ImGui::SameLine();

        const float offset_y = 6;
        const float offset_x = 4;
        const auto pos = ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2(offset_x, offset_y) - ImVec2(0, ImGui::GetScrollY());
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
                restore_camera_state();
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

            ImGui::SameLine();
            bool sync = _sync;
            if (ImGui::Checkbox(Names::sync.c_str(), &sync))
            {
                set_sync(sync);
            }

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
                    const std::shared_ptr<IFlyby> f_ptr = std::static_pointer_cast<IFlyby>(flyby.lock());
                    set_local_selected_flyby(f_ptr);
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
            if (ImGui::BeginChild("##flybynodelist", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
            {
                _node_filters.render();
                ImGui::SameLine();

                _node_filters.render_settings();

                auto all_nodes = selected_flyby->nodes();
                auto filtered_nodes = all_nodes | 
                    std::views::filter([&](auto&& node)
                        {
                            const auto node_ptr = node.lock();
                            return !(!node_ptr || !_node_filters.match(*node_ptr));
                        }) |
                    std::views::transform([](auto&& node) { return node.lock(); }) | std::ranges::to<std::vector>();

                RowCounter counter{ "camera", all_nodes.size() };
                _node_filters.render_table(filtered_nodes, all_nodes, _selected_node, counter,
                    [&](auto&& node)
                    {
                        const std::shared_ptr<IFlybyNode> f_ptr = std::static_pointer_cast<IFlybyNode>(node.lock());
                        set_local_selected_flyby_node(f_ptr);
                        if (_sync)
                        {
                            messages::send_select_flyby_node(_messaging, f_ptr);
                        }
                    }, {});

                ImGui::EndChild();
            }

            ImGui::SameLine();

            auto selected = _selected_node.lock();
            if (ImGui::BeginChild("Node Details", ImVec2(), true) && selected)
            {
                if (ImGui::BeginTable(Names::stats_listbox.c_str(), 2, 0, ImVec2(-1, 0)))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableNextRow();

                    add_stat("#", selected->number());
                    const auto pos = selected->position() * trlevel::Scale;
                    add_stat("Position", std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z));
                    const auto dir = selected->direction() * trlevel::Scale;
                    add_stat("Direction", std::format("{:.0f}, {:.0f}, {:.0f}", dir.x, dir.y, dir.z));
                    add_stat("Roll", selected->roll());
                    add_stat("Speed", selected->speed());
                    add_stat("Fov", selected->fov());
                    add_stat("Timer", selected->timer());
                    add_stat("Room", selected->room());

                    for (int i = 0; i < 16; ++i)
                    {
                        bool state = (selected->flags() & (1 << i)) != 0;
                        const auto label = flag_name(_platform_and_version, i);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text(label.c_str());
                        ImGui::TableNextColumn();
                        ImGui::BeginDisabled();
                        ImGui::Checkbox(std::format("##{}_state", label).c_str(), &state);
                        ImGui::EndDisabled();
                    }

                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();
        }
    }

    void CameraSinkWindow::capture_camera_state()
    {
        restore_camera_state();
        if (auto camera = _camera.lock())
        {
            IFlyby::CameraState existing_state;
            existing_state.fov = camera->fov();
            existing_state.roll = camera->rotation_roll();
            existing_state.mode = camera->mode();
            _initial_state = existing_state;
        }
    }

    void CameraSinkWindow::restore_camera_state()
    {
        if (!_initial_state.has_value())
        {
            return;
        }

        if (auto camera = _camera.lock())
        {
            camera->set_fov(_initial_state->fov);
            camera->set_rotation_roll(_initial_state->roll);
            camera->set_mode(_initial_state->mode);
            camera->set_rotation_yaw(camera->rotation_yaw());
        }

        _initial_state.reset();
    }

    void CameraSinkWindow::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
            if (!_columns_set)
            {
                _filters.set_columns(_settings->camera_sink_window_columns);
                _flyby_filters.set_columns(_settings->flyby_columns);
                _node_filters.set_columns(_settings->flyby_node_columns);
                _columns_set = true;
            }
        }
        else if (auto selected_camera_sink = messages::read_select_camera_sink(message))
        {
            set_selected_camera_sink(selected_camera_sink.value());
        }
        else if (auto selected_flyby_node = messages::read_select_flyby_node(message))
        {
            set_selected_flyby_node(selected_flyby_node.value());
        }
        else if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                _level = level.value();
                set_camera_sinks(level_ptr->camera_sinks());
                set_flybys(level_ptr->flybys());
                set_platform_and_version(level_ptr->platform_and_version());
            }
        }
        else if (auto selected_room = messages::read_select_room(message))
        {
            set_current_room(selected_room.value());
        }
    }

    void CameraSinkWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
        messages::get_selected_room(_messaging, weak_from_this());
        messages::get_selected_camera_sink(_messaging, weak_from_this());
        messages::get_selected_flyby_node(_messaging, weak_from_this());
    }

    std::string CameraSinkWindow::type() const
    {
        return "CameraSink";
    }

    std::string CameraSinkWindow::title() const
    {
        return _id;
    }
}
