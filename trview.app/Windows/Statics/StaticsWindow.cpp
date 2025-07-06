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

        _filters.set_columns(std::vector<std::string>{ "#", "Room", "ID", "Type", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Room", "ID", "Type", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                _settings.statics_window_columns = _filters.columns();
                on_settings(_settings);
            };
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
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void StaticsWindow::render_statics_list()
    {
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

            ImGui::SameLine();
            _filters.render_settings();

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

            RowCounter counter{ "static", _all_statics.size() };

            _filters.render_table(filtered_statics, _all_statics, _selected_static_mesh, counter,
                [&](auto&& stat)
                {
                    set_local_selected_static_mesh(stat);
                    if (_sync_static)
                    {
                        on_static_selected(stat);
                    }
                },
                {
                    {
                        "Hide",
                        {
                            .on_toggle = [&](auto&& static_mesh, auto&& value)
                                {
                                    if (auto static_mesh_ptr = static_mesh.lock())
                                    {
                                        static_mesh_ptr->set_visible(!value);
                                    }
                                },
                            .on_toggle_all = [&](bool value)
                                {
                                    std::ranges::for_each(filtered_statics, [=](auto&& static_mesh) { static_mesh->set_visible(!value); });
                                },
                            .all_toggled = [&]() { return std::ranges::all_of(filtered_statics, [](auto&& static_mesh) { return !static_mesh->visible(); }); }
                        }
                    }
                });
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

    void StaticsWindow::set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics)
    {
        _all_statics = statics;
        setup_filters();
        _filters.force_sort();
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

        _filters.add_getter<int>("#", [](auto&& stat) { return static_cast<int>(stat.number()); });
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& stat) { return to_string(stat.type()); });
        _filters.add_getter<int>("X", [](auto&& stat) { return static_cast<int>(stat.position().x * trlevel::Scale_X); });
        _filters.add_getter<int>("Y", [](auto&& stat) { return static_cast<int>(stat.position().y * trlevel::Scale_Y); });
        _filters.add_getter<int>("Z", [](auto&& stat) { return static_cast<int>(stat.position().z * trlevel::Scale_Z); });
        _filters.add_getter<float>("Rotation", [](auto&& stat) { return static_cast<float>(DirectX::XMConvertToDegrees(stat.rotation())); });
        _filters.add_getter<int>("ID", [](auto&& stat) { return static_cast<int>(stat.id()); });
        _filters.add_getter<int>("Room", [](auto&& stat) { return static_cast<int>(static_mesh_room(stat)); });
        _filters.add_getter<bool>("Breakable", [](auto&& item) { return item.breakable(); });
        _filters.add_getter<std::string>("Flags", [](auto&& stat) { return format_binary(stat.flags()); });
        _filters.add_getter<bool>("Has Collision", [](auto&& stat) { return stat.has_collision(); });
        _filters.add_getter<bool>("Hide", [](auto&& stat) { return !stat.visible(); }, EditMode::ReadWrite);
    }

    void StaticsWindow::set_local_selected_static_mesh(std::weak_ptr<IStaticMesh> static_mesh)
    {
        _selected_static_mesh = static_mesh;
        _filters.force_sort();
    }

    void StaticsWindow::set_sync_static(bool value)
    {
        if (_sync_static != value)
        {
            _sync_static = value;
            _filters.scroll_to_item();
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
            _filters.scroll_to_item();
            set_local_selected_static_mesh(static_mesh);
        }
    }

    void StaticsWindow::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        if (!_columns_set)
        {
            _filters.set_columns(settings.statics_window_columns);
            _columns_set = true;
        }
    }
}
