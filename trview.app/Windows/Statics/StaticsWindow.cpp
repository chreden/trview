#include "StaticsWindow.h"
#include "../RowCounter.h"
#include "../../trview_imgui.h"

namespace trview
{
    IStaticsWindow::~IStaticsWindow()
    {
    }

    StaticsWindow::StaticsWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        setup_filters();
    }

    void StaticsWindow::render()
    {
        if (!render_statics_window())
        {
            on_window_closed();
            return;
        }
    }

    bool StaticsWindow::render_statics_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_statics_list();
            ImGui::SameLine();
            render_static_details();
            _force_sort = false;
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void StaticsWindow::render_statics_list()
    {
        calculate_column_widths();
        if (ImGui::BeginChild(Names::statics_list_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _auto_hider.check_focus();
            _filters.render();

            ImGui::SameLine();
            _track.render();

            ImGui::SameLine();
            bool sync_static = _sync_static;
            if (ImGui::Checkbox(Names::sync_item.c_str(), &sync_static))
            {
                set_sync_static(sync_static);
            }

            _auto_hider.render();

            auto filtered_statics =
                _all_statics |
                std::views::filter([&](auto&& stat)
                    {
                        const auto stat_ptr = stat.lock();
                        return !(!stat_ptr || (_track.enabled<Type::Room>() && stat_ptr->room().lock() != _current_room.lock() || !_filters.match(*stat_ptr)));
                    }) |
                std::views::transform([](auto&& stat) { return stat.lock(); }) |
                std::ranges::to<std::vector>();

            _auto_hider.apply(_all_statics, filtered_statics, _filters);

            RowCounter counter{ "statics", _all_statics.size() };
            if (ImGui::BeginTable(Names::statics_list.c_str(), 5, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, -counter.height())))
            {
                imgui_header_row(
                    {
                        { "#", _column_sizer.size(0) },
                        { "Room", _column_sizer.size(1) },
                        { "ID", _column_sizer.size(2) },
                        { "Type", _column_sizer.size(3) },
                        {.name = "Hide", .width = _column_sizer.size(4), .set_checked = [&](bool v)
                            {
                                std::ranges::for_each(filtered_statics, [=](auto&& stat) { stat->set_visible(!v); });
                            }, .checked = std::ranges::all_of(filtered_statics, [](auto&& stat) { return !stat->visible(); })
                        }
                    });

                imgui_sort_weak(_all_statics,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(static_mesh_room(l), l.number()) < std::tuple(static_mesh_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.id(), l.number()) < std::tuple(r.id(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.type(), l.number()) < std::tuple(r.type(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& stat : filtered_statics)
                {
                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto selected_static_mesh = _selected_static_mesh.lock();
                    bool selected = selected_static_mesh && selected_static_mesh == stat;

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_static)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_static = false;
                    }

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}##{0}", stat->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();

                        set_local_selected_static_mesh(stat);
                        if (_sync_static)
                        {
                            on_static_selected(stat);
                        }
                        _scroll_to_static = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(static_mesh_room(*stat)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(stat->id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(stat->type()).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !stat->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", stat->number()).c_str(), &hidden))
                    {
                        stat->set_visible(!hidden);
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
                counter.render();
            }
        }
        ImGui::EndChild();
    }

    void StaticsWindow::render_static_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            ImGui::Text("Static Details");
            if (ImGui::BeginTable(Names::static_stats.c_str(), 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                if (auto stat = _selected_static_mesh.lock())
                {
                    auto add_stat = [&]<typename T>(const std::string& name, T&& value)
                    {
                        const auto string_value = get_string(value);
                        ImGui::TableNextColumn();
                        ImGui::Text(name.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(string_value.c_str());
                        if (ImGui::BeginPopupContextItem(name.c_str()))
                        {
                            if (ImGui::MenuItem("Copy"))
                            {
                                _clipboard->write(to_utf16(string_value));
                            }
                            ImGui::EndPopup();
                        }
                    };

                    auto position_text = [&stat]()
                        {
                            const auto pos = stat->position() * trlevel::Scale;
                            return std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z);
                        };

                    add_stat("#", stat->number());
                    add_stat("Position", position_text());
                    add_stat("ID", stat->id());
                    add_stat("Room", static_mesh_room(stat));
                    add_stat("Type", to_string(stat->type()));
                    add_stat("Rotation", DirectX::XMConvertToDegrees(stat->rotation()));
                    add_stat("Flags", format_binary(stat->flags()));
                    add_stat("Breakable", stat->breakable());
                    add_stat("Has Collision", stat->has_collision());
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    std::weak_ptr<IStaticMesh> StaticsWindow::selected_static() const
    {
        return _selected_static_mesh;
    }

    void StaticsWindow::set_number(int32_t number)
    {
        _id = std::format("Statics {}", number);
    }

    void StaticsWindow::update(float)
    {
    }

    void StaticsWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("ID__", 2);
        _column_sizer.measure("Type__", 3);
        _column_sizer.measure("Hide________", 4);

        for (const auto& stat : _all_statics)
        {
            if (auto stat_ptr = stat.lock())
            {
                _column_sizer.measure(std::format("{0}##{0}", stat_ptr->number()), 0);
                _column_sizer.measure(std::to_string(static_mesh_room(*stat_ptr)), 1);
                _column_sizer.measure(std::to_string(stat_ptr->id()), 2);
                _column_sizer.measure(to_string(stat_ptr->type()), 3);
            }
        }
    }

    void StaticsWindow::set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics)
    {
        _all_statics = statics;
        setup_filters();
        _force_sort = true;
        calculate_column_widths();
    }

    void StaticsWindow::setup_filters()
    {
        _filters.clear_all_getters();

        std::set<std::string> available_types;
        for (const auto& stat : _all_statics)
        {
            if (auto stat_ptr = stat.lock())
            {
                available_types.insert(to_string(stat_ptr->type()));
            }
        }
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& stat) { return to_string(stat.type()); });
        _filters.add_getter<float>("#", [](auto&& stat) { return static_cast<float>(stat.number()); });
        _filters.add_getter<float>("X", [](auto&& stat) { return stat.position().x * trlevel::Scale_X; });
        _filters.add_getter<float>("Y", [](auto&& stat) { return stat.position().y * trlevel::Scale_Y; });
        _filters.add_getter<float>("Z", [](auto&& stat) { return stat.position().z * trlevel::Scale_Z; });
        _filters.add_getter<float>("Rotation", [](auto&& stat) { return static_cast<float>(DirectX::XMConvertToDegrees(stat.rotation())); });
        _filters.add_getter<float>("ID", [](auto&& stat) { return static_cast<float>(stat.id()); });
        _filters.add_getter<float>("Room", [](auto&& stat) { return static_cast<float>(static_mesh_room(stat)); });
        _filters.add_getter<bool>("Breakable", [](auto&& item) { return item.breakable(); });
        _filters.add_getter<std::string>("Flags", [](auto&& stat) { return format_binary(stat.flags()); });
        _filters.add_getter<bool>("Has Collision", [](auto&& stat) { return stat.has_collision(); });
    }

    void StaticsWindow::set_local_selected_static_mesh(std::weak_ptr<IStaticMesh> static_mesh)
    {
        _selected_static_mesh = static_mesh;
        _force_sort = true;
    }

    void StaticsWindow::set_sync_static(bool value)
    {
        if (_sync_static != value)
        {
            _sync_static = value;
            _scroll_to_static = true;
            if (_sync_static && _global_selected_static.lock())
            {
                set_selected_static(_global_selected_static);
            }
        }
    }

    void StaticsWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
    }

    void StaticsWindow::set_selected_static(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        _global_selected_static = static_mesh;
        if (_sync_static)
        {
            _scroll_to_static = true;
            set_local_selected_static_mesh(static_mesh);
        }
    }
}
