#include "RoomsWindow.h"
#include <trview.app/Elements/IRoom.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.common/Strings.h>
#include "../trview_imgui.h"
#include "../Elements/Floordata.h"
#include "RowCounter.h"
#include "../Elements/ILevel.h"
#include "../Messages/Messages.h"

namespace trview
{
    namespace
    {
        std::string material_name(uint16_t material)
        {
            switch (material)
            {
            case 0: return "Mud";
            case 1: return "Snow";
            case 2: return "Sand";
            case 3: return "Gravel";
            case 4: return "Ice";
            case 5: return "Water";
            case 6: return "Stone";
            case 7: return "Wood";
            case 8: return "Metal";
            case 9: return "Marble";
            case 10: return "Grass";
            case 11: return "Concrete";
            case 12: return "Old Wood";
            case 13: return "Old Metal";
            }
            return std::to_string(material);
        }

        bool room_is_no_space(const IRoom& room)
        {
            for (const auto& sector : room.sectors())
            {
                if (is_no_space(sector->flags()))
                {
                    return true;
                }
            }
            return false;
        }

        void add_room_flags(IClipboard& clipboard, trlevel::LevelVersion version, const IRoom& room)
        {
            using trlevel::LevelVersion;
            const auto add_flag = [&](const std::string& name, bool flag) 
            {
                auto value = std::format("{}", flag);
                ImGui::TableNextColumn();
                if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                {
                    clipboard.write(to_utf16(value));
                }
                ImGui::TableNextColumn();
                ImGui::Text(value.c_str());
            };
            const auto add_flag_min = [&](LevelVersion min_version, const std::string& name, const std::string& alt_name, bool flag)
            {
                add_flag(version >= min_version ? name : alt_name, flag);
            };

            add_flag("Water", room.water());
            add_flag("Bit 1", room.flag(IRoom::Flag::Bit1));
            add_flag("Bit 2", room.flag(IRoom::Flag::Bit2));
            add_flag_min(LevelVersion::Tomb2, "Outside / 3", "Bit 3", room.outside());
            add_flag("Bit 4", room.flag(IRoom::Flag::Bit4));
            add_flag_min(LevelVersion::Tomb2, "Wind / 5", "Bit 5", room.flag(IRoom::Flag::Wind));
            add_flag("Bit 6", room.flag(IRoom::Flag::Bit6));
            if (version == LevelVersion::Tomb3)
            {
                add_flag("Quicksand / 7", room.flag(IRoom::Flag::Quicksand));
            }
            else if (version > LevelVersion::Tomb3)
            {
                add_flag("Block Lens Flare / 7", room.flag(IRoom::Flag::NoLensFlare));
            }
            else
            {
                add_flag("Bit 7", room.flag(IRoom::Flag::Bit7));
            }
            add_flag_min(LevelVersion::Tomb3, "Caustics / 8", "Bit 8", room.flag(IRoom::Flag::Caustics));
            add_flag_min(LevelVersion::Tomb3, "Reflectivity / 9", "Bit 9", room.flag(IRoom::Flag::WaterReflectivity));
            add_flag_min(LevelVersion::Tomb4, "Snow (NGLE) / 10", "Bit 10", room.flag(IRoom::Flag::Bit10));
            add_flag_min(LevelVersion::Tomb4, "D / Rain / 11", "Bit 11", room.flag(IRoom::Flag::Bit11));
            add_flag_min(LevelVersion::Tomb4, "P / Cold / 12", "Bit 12", room.flag(IRoom::Flag::Bit12));
            add_flag("Bit 13", room.flag(IRoom::Flag::Bit13));
            add_flag("Bit 14", room.flag(IRoom::Flag::Bit14));
            add_flag("Bit 15", room.flag(IRoom::Flag::Bit15));
            add_flag("No Space", room_is_no_space(room));
        }

        float ambient_percentage(int16_t ambient)
        {
            return 1.0f - static_cast<float>(ambient) / static_cast<float>(0x1fff);
        }

        std::size_t item_count(std::vector<std::weak_ptr<IItem>>& items, const IRoom& room)
        {
            return std::ranges::count_if(items, [&room](const auto& item)
                {
                    if (auto i = item.lock())
                    {
                        return item_room(i) == room.number();
                    }
                    return false;
                });
        }
    }

    void add_room_filters(Filters2& filters, const std::weak_ptr<ILevel>& level)
    {
        if (filters.has_type_key("IRoom"))
        {
            return;
        }

        const auto level_ptr = level.lock();
        if (!level_ptr)
        {
            return;
        }

        const auto platform_and_version = level_ptr->platform_and_version();
        const auto level_version = platform_and_version.version;
        const auto trng = level_ptr->trng();

        auto room_getters = Filters2::GettersBuilder()
            .with_type_key("IRoom")
            .with_getter<IRoom, int>("#", [](auto&& room) { return static_cast<int>(room.number()); })
            .with_getter<IRoom, int>("Alternate", [](auto&& room) { return room.alternate_room(); }, [](auto&& room) { return room.alternate_mode() != IRoom::AlternateMode::None; })
            .with_getter<IRoom, int>("#", [](auto&& room) { return static_cast<int>(room.number()); })
            .with_getter<IRoom, int>("X size", [](auto&& room) { return static_cast<int>(room.num_x_sectors()); })
            .with_getter<IRoom, int>("Z size", [](auto&& room) { return static_cast<int>(room.num_z_sectors()); })
            .with_getter<IRoom, int>("X", [](auto&& room) { return static_cast<int>(room.info().x); })
            .with_getter<IRoom, int>("Y", [](auto&& room) { return static_cast<int>(room.info().yBottom); })
            .with_getter<IRoom, int>("Z", [](auto&& room) { return static_cast<int>(room.info().z); })
            .with_getter<IRoom, int>("Triggers", [](auto&& room) { return static_cast<int>(room.triggers().size()); })
            .with_getter<IRoom, int>("Statics", [](auto&& room) { return static_cast<int>(room.static_meshes().size()); })
            .with_getter<IRoom, int>("Items", [&](auto&& room) { return static_cast<int>(room.items().size()); })
            .with_multi_getter<IRoom, float>("Neighbours", [](auto&& room)
                {
                    std::vector<float> results;
                    const auto neighbours = room.neighbours();
                    std::copy(neighbours.begin(), neighbours.end(), std::back_inserter(results));
                    return results;
                })
            .with_multi_getter<IRoom, float>("Trigger Index", [&](auto&& room)
                {
                    std::vector<float> results;
                    for (const auto& trigger : room.triggers())
                    {
                        if (const auto trigger_ptr = trigger.lock())
                        {
                            results.push_back(static_cast<float>(trigger_ptr->number()));
                        }
                    }
                    return results;
                });
        
        std::set<std::string> available_trigger_types;
        for (const auto& room : level_ptr->rooms())
        {
            if (auto room_ptr = room.lock())
            {
                for (const auto& trigger : room_ptr->triggers())
                {
                    if (auto trigger_ptr = trigger.lock())
                    {
                        available_trigger_types.insert(to_string(trigger_ptr->type()));
                    }
                }
            }
        }
        room_getters
            .with_multi_getter<IRoom, std::string>("Trigger Type", { available_trigger_types.begin(), available_trigger_types.end() }, [&](auto&& room)
            {
                std::vector<std::string> results;
                for (const auto& trigger : room.triggers())
                {
                    if (const auto trigger_ptr = trigger.lock())
                    {
                        results.push_back(to_string(trigger_ptr->type()));
                    }
                }
                return results;
            })
            .with_multi_getter<IRoom, int>("Item Index", [&](auto&& room)
            {
                std::vector<int> results;
                for (const auto& item : room.items())
                {
                    if (const auto item_ptr = item.lock())
                    {
                        results.push_back(static_cast<int>(item_ptr->number()));
                    }
                }
                return results;
            });

        std::set<std::string> available_item_types;
        for (const auto& item : level_ptr->items())
        {
            if (auto item_ptr = item.lock())
            {
                available_item_types.insert(item_ptr->type());
            }
        }
        room_getters
            .with_multi_getter<IRoom, std::string>("Item Type", { available_item_types.begin(), available_item_types.end() }, [&](auto&& room)
            {
                std::vector<std::string> results;
                for (const auto& item : room.items())
                {
                    if (auto item_ptr = item.lock())
                    {
                        results.push_back(item_ptr->type());
                    }
                }
                return results;
            })
            .with_getter<IRoom, bool>("Hide", [](auto&& room) { return !room.visible(); }, EditMode::ReadWrite)
            .with_getter<IRoom, bool>("Water", [](auto&& room) { return room.water(); });

        if (level_version >= trlevel::LevelVersion::Tomb3)
        {
            room_getters.with_getter<IRoom, int>("Water Scheme", [](auto&& room) { return room.water_scheme(); });
        }

        room_getters.with_getter<IRoom, bool>("Bit 1", [](auto&& room) { return room.flag(IRoom::Flag::Bit1); })
            .with_getter<IRoom, bool>("Bit 2", [](auto&& room) { return room.flag(IRoom::Flag::Bit2); })
            .with_getter<IRoom, bool>("Outside/Bit 3", [](auto&& room) { return room.outside(); })
            .with_getter<IRoom, bool>("Bit 4", [](auto&& room) { return room.flag(IRoom::Flag::Bit4); })
            .with_getter<IRoom, bool>("Wind/Bit 5", [](auto&& room) { return room.flag(IRoom::Flag::Wind); })
            .with_getter<IRoom, bool>("Bit 6", [](auto&& room) { return room.flag(IRoom::Flag::Bit6); })
            .with_getter<IRoom, bool>("Quicksand/Block Lens Flare/Bit 7", [](auto&& room) { return room.flag(IRoom::Flag::Bit7); })
            .with_getter<IRoom, bool>("Caustics/Bit 8", [](auto&& room) { return room.flag(IRoom::Flag::Caustics); })
            .with_getter<IRoom, bool>("Reflectivity/Bit 9", [](auto&& room) { return room.flag(IRoom::Flag::WaterReflectivity); })
            .with_getter<IRoom, bool>("Snow (NGLE)/Bit 10", [](auto&& room) { return room.flag(IRoom::Flag::Bit10); })
            .with_getter<IRoom, bool>("D/Rain/Bit 11", [](auto&& room) { return room.flag(IRoom::Flag::Bit11); })
            .with_getter<IRoom, bool>("P/Cold/Bit 12", [](auto&& room) { return room.flag(IRoom::Flag::Bit12); })
            .with_getter<IRoom, bool>("Bit 13", [](auto&& room) { return room.flag(IRoom::Flag::Bit13); })
            .with_getter<IRoom, bool>("Bit 14", [](auto&& room) { return room.flag(IRoom::Flag::Bit14); })
            .with_getter<IRoom, bool>("Bit 15", [](auto&& room) { return room.flag(IRoom::Flag::Bit15); })
            .with_getter<IRoom, int>("Alternate", [](auto&& room) { return room.alternate_room(); }, [](auto&& room) { return room.alternate_mode() != IRoom::AlternateMode::None; })
            .with_getter<IRoom, int>("Alternate Group", [](auto&& room) { return room.alternate_group(); }, [](auto&& room) { return room.alternate_mode() != IRoom::AlternateMode::None; })
            .with_getter<IRoom, bool>("No Space", room_is_no_space);

        if (level_version < trlevel::LevelVersion::Tomb4)
        {
            room_getters.with_getter<IRoom, int>("Ambient Intensity", [](auto&& room) { return room.ambient_intensity_1(); });
            room_getters.with_getter<IRoom, float>("Ambient Intensity %%", [](auto&& room) { return ambient_percentage(room.ambient_intensity_1()) * 100.0f; });
            if (level_version > trlevel::LevelVersion::Tomb1)
            {
                if (level_version == trlevel::LevelVersion::Tomb2)
                {
                    room_getters.with_getter<IRoom, int>("Ambient Intensity 2", [](auto&& room) { return room.ambient_intensity_2(); });
                    room_getters.with_getter<IRoom, float>("Ambient Intensity 2 %%", [](auto&& room) { return ambient_percentage(room.ambient_intensity_2()) * 100.0f; });
                }
                room_getters.with_getter<IRoom, int>("Light Mode", [](auto&& room) { return room.light_mode(); });
            }
        }
        else
        {
            room_getters.with_getter<IRoom, float>("Ambient R", [](auto&& room) { return room.ambient().r; });
            room_getters.with_getter<IRoom, float>("Ambient G", [](auto&& room) { return room.ambient().g; });
            room_getters.with_getter<IRoom, float>("Ambient B", [](auto&& room) { return room.ambient().b; });
        }

        const auto available_floordata_types =
            std::views::iota(std::to_underlying(Floordata::Command::Function::None) + 1, std::to_underlying(Floordata::Command::Function::Count) + 0)
            | std::views::transform([](auto c) { return to_string(static_cast<Floordata::Command::Function>(c)); })
            | std::ranges::to<std::set>();

        const auto floordata = level_ptr->floor_data();
        room_getters.with_multi_getter<IRoom, std::string>("Floordata Type", { available_floordata_types.begin(), available_floordata_types.end() }, [=](auto&& room)
            {
                const auto& sectors = room.sectors();
                return sectors
                    | std::views::transform([&](auto&& s) { return parse_floordata(floordata, s->floordata_index(), FloordataMeanings::None, trng, platform_and_version).commands; })
                    | std::views::join
                    | std::views::transform([](auto&& c) { return c.type; })
                    | std::ranges::to<std::unordered_set>()
                    | std::views::transform([](auto&& s) { return to_string(s); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Floordata Index", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->floordata_index()); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Material", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->material()); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Box Index", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->box_index()); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, bool>("Stopper", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return s->stopper(); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Room Below", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->room_below()); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Floor", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->floor()); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Room Above", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->room_above()); })
                    | std::ranges::to<std::vector>();
            })
            .with_multi_getter<IRoom, int>("Ceiling", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->ceiling()); })
                    | std::ranges::to<std::vector>();
            })
            .with_getter<IRoom, bool>("In Visible Room", [](auto&& room)
            {
                if (const auto level = room.level().lock())
                {
                    return level->is_in_visible_set(level->room(room.number()));
                }
                return false;
            });

        filters.add_getters(room_getters.build<IRoom>());
    }

    RoomsWindow::RoomsWindow(const IMapRenderer::Source& map_renderer_source, const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging)
        : _map_renderer(map_renderer_source()), _clipboard(clipboard), _messaging(messaging)
    {
        _token_store += _map_renderer->on_sector_selected += [&](auto sector) { _local_selected_sector = sector; };

        generate_filters();

        _filters.set_columns(std::vector<std::string>{ "#", "Items", "Triggers", "Statics", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Items", "Triggers", "Statics", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->rooms_window_columns = _filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };

        _token_store += _track.on_toggle<Type::Item>() += [&](bool value)
        {
            if (value && _global_selected_item.lock())
            {
                set_selected_item(_global_selected_item);
            }
        };
        _token_store += _track.on_toggle<Type::Trigger>() += [&](bool) { set_selected_trigger(_global_selected_trigger); };
        _token_store += _track.on_toggle<Type::Light>() += [&](bool) { set_selected_light(_global_selected_light); };
        _token_store += _track.on_toggle<Type::CameraSink>() += [&](bool) { set_selected_camera_sink(_global_selected_camera_sink); };
        _token_store += _track.on_toggle<Type::Sector>() += [&](bool) { set_selected_sector(_global_selected_sector); };
    }

    void RoomsWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        auto room_ptr = room.lock();
        auto current = _current_room.lock();
        if (room_ptr == current)
        {
            return;
        }

        _local_selected_sector.reset();
        _current_room = room;
        _filters.scroll_to_item();
        if (_sync_room && room_ptr)
        {
            select_room(_current_room);
            load_room_details(_current_room);
        }
    }

    void RoomsWindow::load_room_details(std::weak_ptr<IRoom> room)
    {
        room;
        // _map_renderer->load(room.lock());
    }

    void RoomsWindow::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _all_items = items;
        _global_selected_item.reset();
        _local_selected_item.reset();
        _force_sort = true;
        _filters.force_sort();
    }

    void RoomsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void RoomsWindow::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
        _current_room.reset();
        _triggers.clear();
        _lights.clear();
        _camera_sinks.clear();
        generate_filters();
        _force_sort = true;
        _filters.force_sort();
        _local_selected_sector.reset();
        _map_renderer->set_selection(std::vector<std::shared_ptr<ISector>>{});
    }

    void RoomsWindow::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        _global_selected_item = item;
        if (_track.enabled<Type::Item>())
        {
            if (_selected_room.lock()!= _current_room.lock())
            {
                if (auto item_ptr = item.lock())
                {
                    select_room(item_ptr->room());
                    _filters.scroll_to_item();
                    load_room_details(item_ptr->room());
                }
            }

            _local_selected_item = item;
            _scroll_to_item = true;
        }
    }

    void RoomsWindow::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _global_selected_trigger = trigger;
        if (_track.enabled<Type::Trigger>())
        {
            if (const auto trigger_ptr = trigger.lock())
            {
                if (_selected_room.lock() != _current_room.lock())
                {
                    select_room(trigger_ptr->room());
                    _filters.scroll_to_item();
                    load_room_details(trigger_ptr->room());
                }

                _local_selected_trigger = trigger;
                _scroll_to_trigger = true;
            }
        }
    }

    void RoomsWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _local_selected_trigger.reset();
        _triggers = triggers;
        _force_sort = true;
        _filters.force_sort();
    }

    void RoomsWindow::set_sync_room(bool value)
    {
        if (_sync_room != value)
        {
            _sync_room = value;
            auto room = _current_room;
            if (_sync_room)
            {
                // Force room to be different to current room so that the
                // room details are loaded.
                _current_room.reset();
            }
            set_current_room(room);
        }
    }

    void RoomsWindow::render()
    {
        if (!_settings)
        {
            messages::get_settings(_messaging, weak_from_this());
        }

        if (!render_rooms_window())
        {
            on_window_closed();
            return;
        }
    }

    bool RoomsWindow::render_rooms_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(620, 760));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_rooms_list();
            ImGui::SameLine();
            render_room_details();
            _force_sort = false;

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

    void RoomsWindow::render_rooms_list()
    {
        if (ImGui::BeginChild(Names::rooms_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _auto_hider.check_focus();

            _filters.render();
            bool filter_changed = _filters.test_and_reset_changed();

            ImGui::SameLine();
            _track.render();

            ImGui::SameLine();
            bool sync_room = _sync_room;
            if (ImGui::Checkbox("Sync##syncroom", &sync_room))
            {
                set_sync_room(sync_room);
            }

            _auto_hider.render();

            auto filtered_rooms =
                _all_rooms |
                std::views::filter([&](auto&& room)
                    {
                        const auto room_ptr = room.lock();
                        return !(!room_ptr || !_filters.match(*room_ptr));
                    }) |
                std::views::transform([](auto&& room) { return room.lock(); }) |
                std::ranges::to<std::vector>();

            _auto_hider.apply(_all_rooms, filtered_rooms, filter_changed);

            ImGui::SameLine();
            _filters.render_settings();

            RowCounter counter{ "room", _all_rooms.size() };

            _filters.render_table(filtered_rooms, _all_rooms, _selected_room, counter,
                [&](auto&& room)
                {
                    select_room(room);
                    _map_renderer->load(room.lock());
                    filter_changed = true;
                    if (_sync_room)
                    {
                        messages::send_select_room(_messaging, _selected_room);
                    }
                }, default_hide(filtered_rooms));

            if (filter_changed)
            {
                apply_sector_filters();
            }
        }
        ImGui::EndChild();
    }

    void RoomsWindow::render_room_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            if (auto room = _selected_room.lock())
            {
                if (_map_renderer->loaded())
                {
                    const auto size = _map_renderer->size();
                    float remainder = (341 - size.height) * 0.5f;
                    ImGui::SetCursorPosX(std::round((ImGui::GetWindowSize().x - size.width) * 0.5f));
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + std::floor(remainder));
                    _map_renderer->render(false);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + std::ceil(remainder));
                    ImGui::Separator();
                }

                if (ImGui::BeginTabBar("TabBar"))
                {
                    if (ImGui::BeginTabItem(Names::properties_tab.c_str()))
                    {
                        render_properties_tab(room);
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Neighbours"))
                    {
                        render_neighbours_tab(room);
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Items", 0, _scroll_to_item ? ImGuiTabItemFlags_SetSelected : 0))
                    {
                        render_items_tab(room);
                        ImGui::EndTabItem();
                        _scroll_to_item = false;
                    }

                    if (ImGui::BeginTabItem("Triggers", 0, _scroll_to_trigger ? ImGuiTabItemFlags_SetSelected : 0))
                    {
                        render_triggers_tab();
                        ImGui::EndTabItem();
                        _scroll_to_trigger = false;
                    }

                    if (ImGui::BeginTabItem("Camera/Sink", 0, _scroll_to_camera_sink ? ImGuiTabItemFlags_SetSelected : 0))
                    {
                        render_camera_sink_tab();
                        ImGui::EndTabItem();
                        _scroll_to_camera_sink = false;
                    }

                    if (ImGui::BeginTabItem("Lights", 0, _scroll_to_light ? ImGuiTabItemFlags_SetSelected : 0))
                    {
                        render_lights_tab();
                        ImGui::EndTabItem();
                        _scroll_to_light = false;
                    }

                    if (ImGui::BeginTabItem("Sector"))
                    {
                        _map_renderer->set_mode(IMapRenderer::Mode::Select);
                        render_sector_tab();
                        ImGui::EndTabItem();
                    }
                    else
                    {
                        _map_renderer->set_mode(IMapRenderer::Mode::Normal);
                    }

                    if (ImGui::BeginTabItem("Statics##Statics Tab"))
                    {
                        render_statics_tab();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
            }
        }
        ImGui::EndChild();
    }

    void RoomsWindow::clear_selected_trigger()
    {
        _local_selected_trigger.reset();
        _global_selected_trigger.reset();
    }

    void RoomsWindow::update(float delta)
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

    void RoomsWindow::set_number(int32_t number)
    {
        _id = "Rooms " + std::to_string(number);
    }

    void RoomsWindow::generate_filters()
    {
        _filters.clear_all_getters();
        _filters.add_getter<int>("#", [](auto&& room) { return static_cast<int>(room.number()); });
        _filters.add_getter<int>("X size", [](auto&& room) { return static_cast<int>(room.num_x_sectors()); });
        _filters.add_getter<int>("Z size", [](auto&& room) { return static_cast<int>(room.num_z_sectors()); });
        _filters.add_getter<int>("X", [](auto&& room) { return static_cast<int>(room.info().x); });
        _filters.add_getter<int>("Y", [](auto&& room) { return static_cast<int>(room.info().yBottom); });
        _filters.add_getter<int>("Z", [](auto&& room) { return static_cast<int>(room.info().z); });
        _filters.add_getter<int>("Triggers", [](auto&& room) { return static_cast<int>(room.triggers().size()); });
        _filters.add_getter<int>("Statics", [](auto&& room) { return static_cast<int>(room.static_meshes().size()); });
        _filters.add_getter<int>("Items", [&](auto&& room) { return static_cast<int>(item_count(_all_items, room)); });
        _filters.add_multi_getter<float>("Neighbours", [](auto&& room)
            {
                std::vector<float> results;
                const auto neighbours = room.neighbours();
                std::copy(neighbours.begin(), neighbours.end(), std::back_inserter(results));
                return results;
            });
        _filters.add_multi_getter<float>("Trigger Index", [&](auto&& room)
            {
                std::vector<float> results;
                for (const auto& trigger : room.triggers())
                {
                    if (const auto trigger_ptr = trigger.lock())
                    {
                        results.push_back(static_cast<float>(trigger_ptr->number()));
                    }
                }
                return results;
            });

        std::set<std::string> available_trigger_types;
        for (const auto& room : _all_rooms)
        {
            if (auto room_ptr = room.lock())
            {
                for (const auto& trigger : room_ptr->triggers())
                {
                    if (auto trigger_ptr = trigger.lock())
                    {
                        available_trigger_types.insert(to_string(trigger_ptr->type()));
                    }
                }
            }
        }
        _filters.add_multi_getter<std::string>("Trigger Type", { available_trigger_types.begin(), available_trigger_types.end() }, [&](auto&& room)
            {
                std::vector<std::string> results;
                for (const auto& trigger : room.triggers())
                {
                    if (const auto trigger_ptr = trigger.lock())
                    {
                        results.push_back(to_string(trigger_ptr->type()));
                    }
                }
                return results;
            });
        _filters.add_multi_getter<float>("Item Index", [&](auto&& room)
            {
                std::vector<float> results;
                for (const auto& item : _all_items)
                {
                    if (const auto item_ptr = item.lock())
                    {
                        if (item_room(item_ptr) == room.number())
                        {
                            results.push_back(static_cast<float>(item_ptr->number()));
                        }
                    }
                }
                return results;
            });

        std::set<std::string> available_item_types;
        for (const auto& item : _all_items)
        {
            if (auto item_ptr = item.lock())
            {
                available_item_types.insert(item_ptr->type());
            }
        }
        _filters.add_multi_getter<std::string>("Item Type", { available_item_types.begin(), available_item_types.end() }, [&](auto&& room)
            {
                std::vector<std::string> results;
                for (const auto& item : _all_items)
                {
                    if (auto item_ptr = item.lock())
                    {
                        if (item_room(item_ptr) == room.number())
                        {
                            results.push_back(item_ptr->type());
                        }
                    }
                }
                return results;
            });
        _filters.add_getter<bool>("Hide", [](auto&& room) { return !room.visible(); }, EditMode::ReadWrite);
        _filters.add_getter<bool>("Water", [](auto&& room) { return room.water(); });
        if (_level_version >= trlevel::LevelVersion::Tomb3)
        {
            _filters.add_getter<int>("Water Scheme", [](auto&& room) { return room.water_scheme(); });
        }
        _filters.add_getter<bool>("Bit 1", [](auto&& room) { return room.flag(IRoom::Flag::Bit1); });
        _filters.add_getter<bool>("Bit 2", [](auto&& room) { return room.flag(IRoom::Flag::Bit2); });
        _filters.add_getter<bool>("Outside/Bit 3", [](auto&& room) { return room.outside(); });
        _filters.add_getter<bool>("Bit 4", [](auto&& room) { return room.flag(IRoom::Flag::Bit4); });
        _filters.add_getter<bool>("Wind/Bit 5", [](auto&& room) { return room.flag(IRoom::Flag::Wind); });
        _filters.add_getter<bool>("Bit 6", [](auto&& room) { return room.flag(IRoom::Flag::Bit6); });
        _filters.add_getter<bool>("Quicksand/Block Lens Flare/Bit 7", [](auto&& room) { return room.flag(IRoom::Flag::Bit7); });
        _filters.add_getter<bool>("Caustics/Bit 8", [](auto&& room) { return room.flag(IRoom::Flag::Caustics); });
        _filters.add_getter<bool>("Reflectivity/Bit 9", [](auto&& room) { return room.flag(IRoom::Flag::WaterReflectivity); });
        _filters.add_getter<bool>("Snow (NGLE)/Bit 10", [](auto&& room) { return room.flag(IRoom::Flag::Bit10); });
        _filters.add_getter<bool>("D/Rain/Bit 11", [](auto&& room) { return room.flag(IRoom::Flag::Bit11); });
        _filters.add_getter<bool>("P/Cold/Bit 12", [](auto&& room) { return room.flag(IRoom::Flag::Bit12); });
        _filters.add_getter<bool>("Bit 13", [](auto&& room) { return room.flag(IRoom::Flag::Bit13); });
        _filters.add_getter<bool>("Bit 14", [](auto&& room) { return room.flag(IRoom::Flag::Bit14); });
        _filters.add_getter<bool>("Bit 15", [](auto&& room) { return room.flag(IRoom::Flag::Bit15); });
        _filters.add_getter<int>("Alternate", [](auto&& room) { return room.alternate_room(); }, [](auto&& room) { return room.alternate_mode() != IRoom::AlternateMode::None; });
        _filters.add_getter<int>("Alternate Group", [](auto&& room) { return room.alternate_group(); }, [](auto&& room) { return room.alternate_mode() != IRoom::AlternateMode::None; });
        _filters.add_getter<bool>("No Space", room_is_no_space);
        if (_level_version < trlevel::LevelVersion::Tomb4)
        {
            _filters.add_getter<int>("Ambient Intensity", [](auto&& room) { return room.ambient_intensity_1(); });
            _filters.add_getter<float>("Ambient Intensity %%", [](auto&& room) { return ambient_percentage(room.ambient_intensity_1()) * 100.0f; });
            if (_level_version > trlevel::LevelVersion::Tomb1)
            {
                if (_level_version == trlevel::LevelVersion::Tomb2)
                {
                    _filters.add_getter<int>("Ambient Intensity 2", [](auto&& room) { return room.ambient_intensity_2(); });
                    _filters.add_getter<float>("Ambient Intensity 2 %%", [](auto&& room) { return ambient_percentage(room.ambient_intensity_2()) * 100.0f; });
                }
                _filters.add_getter<int>("Light Mode", [](auto&& room) { return room.light_mode(); });
            }
        }
        else
        {
            _filters.add_getter<float>("Ambient R", [](auto&& room) { return room.ambient().r; });
            _filters.add_getter<float>("Ambient G", [](auto&& room) { return room.ambient().g; });
            _filters.add_getter<float>("Ambient B", [](auto&& room) { return room.ambient().b; });
        }

        const auto available_floordata_types = 
              std::views::iota(std::to_underlying(Floordata::Command::Function::None) + 1, std::to_underlying(Floordata::Command::Function::Count) + 0)
            | std::views::transform([](auto c) { return to_string(static_cast<Floordata::Command::Function>(c)); })
            | std::ranges::to<std::set>();

        _filters.add_multi_getter<std::string>("Floordata Type", { available_floordata_types.begin(), available_floordata_types.end() }, [&](auto&& room)
            {
                const auto& sectors = room.sectors();
                return sectors
                    | std::views::transform([&](auto&& s) { return parse_floordata(_floordata, s->floordata_index(), FloordataMeanings::None, _trng, _platform_and_version).commands; })
                    | std::views::join
                    | std::views::transform([](auto&& c) { return c.type; })
                    | std::ranges::to<std::unordered_set>()
                    | std::views::transform([](auto&& s) { return to_string(s); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Floordata Index", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->floordata_index()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Material", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->material()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Box Index", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->box_index()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<bool>("Stopper", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return s->stopper(); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Room Below", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->room_below()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Floor", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->floor()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Room Above", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->room_above()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<int>("Ceiling", [&](auto&& room)
            {
                return room.sectors()
                    | std::views::transform([&](auto&& s) { return static_cast<int>(s->ceiling()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_getter<bool>("In Visible Room", [](auto&& room)
            {
                if (const auto level = room.level().lock())
                {
                    return level->is_in_visible_set(level->room(room.number()));
                }
                return false;
            });
    }

    void RoomsWindow::render_properties_tab(const std::shared_ptr<IRoom>& room)
    {
        const auto add_stat = [&]<typename T>(const std::string & name, const T && value, std::function<void()> click = {})
        {
            const auto string_value = get_string(value);
            ImGui::TableNextColumn();
            if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (click)
                {
                    click();
                }
                else
                {
                    _clipboard->write(to_utf16(string_value));
                    _tooltip_timer = 0.0f;
                }
            }
            ImGui::TableNextColumn();
            ImGui::Text(string_value.c_str());
        };

        if (ImGui::BeginTable(Names::properties.c_str(), 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Value");
            ImGui::TableNextRow();

            add_stat("X", room->info().x);
            add_stat("Y", room->info().yBottom);
            add_stat("Z", room->info().z);
            if (room->alternate_mode() != Room::AlternateMode::None)
            {
                add_stat("Alternate", room->alternate_room(), [this, room]() 
                    { 
                        if (auto level = room->level().lock())
                        {
                            messages::send_select_room(_messaging, level->room(room->alternate_room()));
                        }
                    });
                if (room->alternate_group() != 0xff)
                {
                    add_stat("Alternate Group", room->alternate_group());
                }
            }

            auto format_colour = [](const Colour& colour)
            {
                return std::format("R: {}, G: {}, B: {}", static_cast<int>(colour.r * 255), static_cast<int>(colour.g * 255), static_cast<int>(colour.b * 255));
            };

            if (_level_version < trlevel::LevelVersion::Tomb4)
            {
                add_stat("Ambient Intensity", room->ambient_intensity_1());
                float ambient_intensity = ambient_percentage(room->ambient_intensity_1());
                ImGui::SameLine();
                ImGui::Text("%.2f%%", ambient_intensity * 100.0f);
                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
                ImGui::ColorButton("##ambientintensitybutton", ImVec4(ambient_intensity, ambient_intensity, ambient_intensity, 1.0f), 0, ImVec2(16, 16));
                if (_level_version > trlevel::LevelVersion::Tomb1)
                {
                    if (_level_version == trlevel::LevelVersion::Tomb2)
                    {
                        add_stat("Ambient Intensity 2", room->ambient_intensity_2());
                        float ambient_intensity2 = ambient_percentage(room->ambient_intensity_2());
                        ImGui::SameLine();
                        ImGui::Text("%.2f%%", ambient_intensity2 * 100.0f);
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
                        ImGui::ColorButton("##ambientintensity2button", ImVec4(ambient_intensity2, ambient_intensity2, ambient_intensity2, 1.0f), 0, ImVec2(16, 16));
                    }
                    add_stat("Light Mode", std::format("{} ({})", room->light_mode(), light_mode_name(room->light_mode())));
                }
            }
            else
            {
                add_stat("Ambient", format_colour(room->ambient()));
                auto ambient = room->ambient();
                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
                ImGui::ColorButton("##ambientbutton", ImVec4(ambient.r, ambient.g, ambient.b, 1.0f), 0, ImVec2(16, 16));
            }

            if (_level_version >= trlevel::LevelVersion::Tomb3)
            {
                add_stat("Water Scheme", room->water_scheme());
            }

            add_room_flags(*_clipboard, _level_version, *room);
            ImGui::EndTable();
        }
    }

    void RoomsWindow::render_neighbours_tab(const std::shared_ptr<IRoom>& room)
    {
        if (ImGui::BeginTable("Neighbours", 1, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& neighbour : room->neighbours())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bool selected = false;
                if (ImGui::Selectable(std::to_string(neighbour).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                {
                    auto level = room->level().lock();
                    if (level)
                    {
                        const auto neighbour_room = level->room(neighbour);
                        select_room(neighbour_room);
                        if (_sync_room)
                        {
                            messages::send_select_room(_messaging, neighbour_room);
                        }
                    }
                }
            }

            ImGui::EndTable();
        }
    }

    void RoomsWindow::render_items_tab(const std::shared_ptr<IRoom>& room)
    {
        if (ImGui::BeginTable("Items", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            imgui_sort_weak(_all_items,
                {
                    [](auto&& l, auto&& r) { return l.number() < r.number(); },
                    [](auto&& l, auto&& r) { return std::tuple(l.type(), l.number()) < std::tuple(r.type(), r.number()); },
                }, _force_sort);

            const auto filtered = _all_items |
                std::views::transform([](auto&& i) { return i.lock(); }) |
                std::views::filter([this, room](auto&& i)
                    {
                        return i && i->room().lock() == room && (i->ng_plus().value_or(_ng_plus) == _ng_plus);
                    }) |
                std::ranges::to<std::vector>();

            for (const auto& item : filtered)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                auto selected_item = _local_selected_item.lock();
                bool selected = selected_item && selected_item == item;

                ImGuiScroller scroller;
                if (selected && _scroll_to_item)
                {
                    scroller.scroll_to_item();
                    _scroll_to_item = false;
                }

                if (ImGui::Selectable(std::to_string(item->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                {
                    scroller.fix_scroll();
                    _local_selected_item = item;
                    messages::send_select_item(_messaging, item);
                    _scroll_to_item = false;
                }

                ImGui::TableNextColumn();
                ImGui::Text(item->type().c_str());
            }

            ImGui::EndTable();
        }
    }

    void RoomsWindow::render_triggers_tab()
    {
        if (ImGui::BeginTable("Triggers", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            imgui_sort_weak(_triggers,
                {
                    [](auto&& l, auto&& r) { return l.number() < r.number(); },
                    [&](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); }
                }, _force_sort);

            for (const auto& trigger : _triggers)
            {
                if (const auto trigger_ptr = trigger.lock())
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const auto local_selection = _local_selected_trigger.lock();
                    bool selected = local_selection && local_selection->number() == trigger_ptr->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_trigger)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_trigger = false;
                    }

                    if (ImGui::Selectable(std::format("{0}##{0}", trigger_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();
                        _local_selected_trigger = trigger_ptr;
                        messages::send_select_trigger(_messaging, trigger);
                        _scroll_to_trigger = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(trigger_ptr->type()).c_str());
                }
            }

            ImGui::EndTable();
        }
    }

    void RoomsWindow::render_sector_tab()
    {
        auto selected_sector = _local_selected_sector.lock();
        if (!selected_sector)
        {
            ImGui::Text("Select a sector to view floordata");
        }
        else 
        {
            const auto selected_room = _selected_room.lock();
            const std::shared_ptr<ILevel> level = selected_room == nullptr ? nullptr : selected_room->level().lock();

            // Some stats table
            const auto add_stat = [&]<typename T>(const std::string & name, const T && value, std::function<void()> click = {})
            {
                const auto string_value = get_string(value);
                ImGui::TableNextColumn();
                if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                {
                    if (click)
                    {
                        click();
                    }
                }
                ImGui::TableNextColumn();
                ImGui::Text(string_value.c_str());
            };

            if (ImGui::BeginTable(Names::properties.c_str(), 2, ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                add_stat("Floordata Index", selected_sector->floordata_index());
                if (_level_version >= trlevel::LevelVersion::Tomb3)
                {
                    add_stat("Material", std::format("{} ({})", selected_sector->material(), material_name(selected_sector->material())));
                }
                add_stat("Box Index", selected_sector->box_index());
                if (_level_version >= trlevel::LevelVersion::Tomb3)
                {
                    add_stat("Stopper", selected_sector->stopper());
                }
                add_stat("Room Below", selected_sector->room_below(), [this, selected_sector, level]()
                    {
                        if (level)
                        {
                            messages::send_select_room(_messaging, level->room(selected_sector->room_below()));
                        }
                    });
                add_stat("Floor", selected_sector->floor());
                add_stat("Room Above", selected_sector->room_above(), [this, selected_sector, level]()
                    {
                        if (level)
                        {
                            messages::send_select_room(_messaging, level->room(selected_sector->room_above()));
                        }
                    });
                add_stat("Ceiling", selected_sector->ceiling());

                ImGui::EndTable();
            }

            ImGui::NewLine();

            ImGui::Checkbox(Names::simple_mode.c_str(), &_simple_mode);
            if (ImGui::BeginTable("##floordata", _simple_mode ? 2 : 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Value");
                if (!_simple_mode)
                {
                    ImGui::TableSetupColumn("Meaning");
                }
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableHeadersRow();

                if (selected_sector)
                {
                    const Floordata floordata = parse_floordata(_floordata, selected_sector->floordata_index(), FloordataMeanings::Generate, _all_items, _trng, _platform_and_version);

                    uint32_t index = selected_sector->floordata_index();
                    for (const auto& command : floordata.commands)
                    {
                        for (std::size_t i = 0; i < command.data.size(); ++i, ++index)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            bool selected = _selected_floordata == index;
                            if (ImGui::Selectable(std::format("{}", index).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                _selected_floordata = index;
                            }
                            if (ImGui::BeginPopupContextItem())
                            {
                                if (ImGui::MenuItem("Copy"))
                                {
                                    _clipboard->write(to_utf16(std::format("{} {:04X} {}", index, command.data[i],
                                        _simple_mode ? "" : command.meanings[i])));
                                }
                                if (ImGui::MenuItem("Copy All"))
                                {
                                    std::string data;
                                    for (uint32_t d = 0; d < command.data.size(); ++d)
                                    {
                                        data += std::format("{} {:04X} {}\n", index, command.data[d],
                                            _simple_mode ? "" : command.meanings[d]);
                                    }
                                    _clipboard->write(to_utf16(data));
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text("%04X", command.data[i]);
                            if (!_simple_mode)
                            {
                                ImGui::TableNextColumn();
                                ImGui::Text(command.meanings[i].c_str());
                            }
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
    }

    void RoomsWindow::set_floordata(const std::vector<uint16_t>& data)
    {
        _floordata = data;
    }

    void RoomsWindow::set_selected_sector(const std::weak_ptr<ISector>& sector)
    {
        _global_selected_sector = sector;
        if (_track.enabled<Type::Sector>())
        {
            if (const auto sector_ptr = sector.lock())
            {
                if (_selected_room.lock() != _current_room.lock())
                {
                    const auto actual = sector_ptr->room();
                    select_room(actual);
                    _filters.scroll_to_item();
                    load_room_details(actual);
                }
                _local_selected_sector = sector;
            }
        }
        _map_renderer->set_selection(_local_selected_sector.lock());
    }

    void RoomsWindow::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        _global_selected_light = light;
        if (_track.enabled<Type::Light>())
        {
            if (const auto light_ptr = light.lock())
            {
                if (_selected_room.lock() != _current_room.lock())
                {
                    select_room(light_ptr->room());
                    _filters.scroll_to_item();
                    load_room_details(light_ptr->room());
                }

                _local_selected_light = light;
                _scroll_to_light = true;
            }
        }
    }
    
    void RoomsWindow::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _global_selected_camera_sink = camera_sink;
        if (_track.enabled<Type::CameraSink>())
        {
            if (const auto camera_sink_ptr = camera_sink.lock())
            {
                if (_selected_room.lock() != _current_room.lock())
                {
                    const auto actual = actual_room(*camera_sink_ptr);
                    select_room(actual);
                    _filters.scroll_to_item();
                    load_room_details(actual);
                }

                _local_selected_camera_sink = camera_sink;
                _scroll_to_camera_sink = true;
            }
        }
    }

    void RoomsWindow::render_camera_sink_tab()
    {
        if (ImGui::BeginTable("Camera/Sinks", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            imgui_sort_weak(_camera_sinks,
                {
                    [](auto&& l, auto&& r) { return l.number() < r.number(); },
                    [&](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); }
                }, _force_sort);

            for (const auto& camera_sink : _camera_sinks)
            {
                if (const auto camera_sink_ptr = camera_sink.lock())
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const auto local_selection = _local_selected_camera_sink.lock();
                    bool selected = local_selection && local_selection->number() == camera_sink_ptr->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_camera_sink)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_camera_sink = false;
                    }

                    if (ImGui::Selectable(std::format("{0}##{0}", camera_sink_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();
                        _local_selected_camera_sink = camera_sink_ptr;
                        messages::send_select_camera_sink(_messaging, camera_sink);
                        _scroll_to_camera_sink = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(camera_sink_ptr->type()).c_str());
                }
            }

            ImGui::EndTable();
        }
    }
    
    void RoomsWindow::render_lights_tab()
    {
        if (ImGui::BeginTable("Lights", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            imgui_sort_weak(_lights,
                {
                    [](auto&& l, auto&& r) { return l.number() < r.number(); },
                    [&](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); }
                }, _force_sort);

            for (const auto& light : _lights)
            {
                if (auto light_ptr = light.lock())
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const auto local_selection = _local_selected_light.lock();
                    bool selected = local_selection && local_selection->number() == light_ptr->number();

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_light)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_light = false;
                    }

                    if (ImGui::Selectable(std::format("{0}##{0}", light_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();
                        _local_selected_light = light_ptr;
                        messages::send_select_light(_messaging, light);
                        _scroll_to_light = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(light_ptr->type()).c_str());
                }
            }

            ImGui::EndTable();
        }
    }

    void RoomsWindow::clear_selected_light()
    {
        _local_selected_light.reset();
        _global_selected_light.reset();
    }

    void RoomsWindow::clear_selected_camera_sink()
    {
        _local_selected_camera_sink.reset();
        _global_selected_camera_sink.reset();
    }

    void RoomsWindow::set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks)
    {
        _local_selected_camera_sink.reset();
        _camera_sinks = camera_sinks;
        _force_sort = true;
    }

    void RoomsWindow::set_lights(const std::vector<std::weak_ptr<ILight>>& lights)
    {
        _local_selected_light.reset();
        _lights = lights;
        _force_sort = true;
    }

    void RoomsWindow::select_room(std::weak_ptr<IRoom> room)
    {
        _selected_room = room;
        _filters.scroll_to_item();
        if (auto room_ptr = room.lock())
        {
            set_triggers(room_ptr->triggers());
            set_lights(room_ptr->lights());
            set_camera_sinks(room_ptr->camera_sinks());
            set_static_meshes(room_ptr->static_meshes());
            return;
        }

        _lights.clear();
        _camera_sinks.clear();
        _triggers.clear();
        _static_meshes.clear();
    }

    void RoomsWindow::render_statics_tab()
    {
        if (ImGui::BeginTable("Statics", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (const auto& static_mesh : _static_meshes)
            {
                if (auto static_mesh_ptr = static_mesh.lock())
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    const auto local_selection = _local_selected_static_mesh.lock();
                    bool selected = local_selection && local_selection == static_mesh_ptr;

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_static_mesh)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_static_mesh = false;
                    }

                    if (ImGui::Selectable(std::format("{0}##{0}", static_mesh_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();
                        _local_selected_static_mesh = static_mesh_ptr;
                        messages::send_select_static_mesh(_messaging, static_mesh);
                        _scroll_to_static_mesh = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(std::format("{}", static_mesh_ptr->id()).c_str());
                }
            }

            ImGui::EndTable();
        }
    }

    void RoomsWindow::set_static_meshes(const std::vector<std::weak_ptr<IStaticMesh>>& static_meshes)
    {
        _local_selected_static_mesh.reset();
        _static_meshes = static_meshes;
        _force_sort = true;
    }

    void RoomsWindow::set_ng_plus(bool value)
    {
        _ng_plus = value;
    }

    void RoomsWindow::set_trng(bool value)
    {
        _trng = value;
    }

    std::string RoomsWindow::name() const
    {
        return _id;
    }

    void RoomsWindow::set_filters(std::vector<Filters<IRoom>::Filter> filters)
    {
        _filters.set_filters(filters);
    }

    void RoomsWindow::receive_message(const Message& message)
    {
        if (auto selected_item = messages::read_select_item(message))
        {
            set_selected_item(selected_item.value());
        }
        else if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
            if (!_columns_set)
            {
                _filters.set_columns(_settings->rooms_window_columns);
                _columns_set = true;
            }
        }
        else if (auto selected_room = messages::read_select_room(message))
        {
            select_room(selected_room.value());
        }
        else if (auto selected_light = messages::read_select_light(message))
        {
            set_selected_light(selected_light.value());
        }
        else if (auto selected_trigger = messages::read_select_trigger(message))
        {
            set_selected_trigger(selected_trigger.value());
        }
        else if (auto selected_camera_sink = messages::read_select_camera_sink(message))
        {
            set_selected_camera_sink(selected_camera_sink.value());
        }
        else if (auto selected_sector = messages::read_select_sector(message))
        {
            set_selected_sector(selected_sector.value());
        }
        else if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                set_rooms(level_ptr->rooms());
                set_items(level_ptr->items());
                set_triggers(level_ptr->triggers());
                set_level_version(level_ptr->version());
                _platform_and_version = level_ptr->platform_and_version();
                set_ng_plus(level_ptr->ng_plus());
                set_trng(level_ptr->trng());
                set_floordata(level_ptr->floor_data());
            }
        }
        else if (auto ng_plus = messages::read_ng_plus(message))
        {
            set_ng_plus(ng_plus.value());
        }
        else if (message.type == "room_filters")
        {
            set_filters(std::static_pointer_cast<MessageData<std::vector<Filters<IRoom>::Filter>>>(message.data)->value);
        }
    }

    void RoomsWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
        messages::get_selected_room(_messaging, weak_from_this());
        messages::get_selected_item(_messaging, weak_from_this());
        messages::get_selected_light(_messaging, weak_from_this());
        messages::get_selected_trigger(_messaging, weak_from_this());
        messages::get_selected_camera_sink(_messaging, weak_from_this());
        messages::get_selected_sector(_messaging, weak_from_this());
    }

    std::optional<Filters<ISector>> RoomsWindow::convert_to_sector_filters() const
    {
        Filters<ISector> sector_filters;
        for (const auto& filter : _filters.filters())
        {
            if (equals_any(filter.key, "Floordata Type", "Floordata Index", "Material", "Box Index", "Stopper", "Room Below", "Floor", "Room Above", "Ceiling"))
            {
                sector_filters.add_filter({ .key = filter.key, .compare = filter.compare, .value = filter.value, .value2 = filter.value, .op = filter.op });
            }
        }

        if (!sector_filters.filters().empty())
        {
            const auto available_floordata_types =
                std::views::iota(std::to_underlying(Floordata::Command::Function::None) + 1, std::to_underlying(Floordata::Command::Function::Count) + 0)
                | std::views::transform([](auto c) { return to_string(static_cast<Floordata::Command::Function>(c)); })
                | std::ranges::to<std::set>();

            sector_filters.add_multi_getter<std::string>("Floordata Type", { available_floordata_types.begin(), available_floordata_types.end() }, [&](auto&& sector)
                {
                    return parse_floordata(_floordata, sector.floordata_index(), FloordataMeanings::None, _trng, _platform_and_version).commands
                        | std::views::transform([](auto&& c) { return c.type; })
                        | std::ranges::to<std::unordered_set>()
                        | std::views::transform([](auto&& s) { return to_string(s); })
                        | std::ranges::to<std::vector>();
                });

            sector_filters.add_getter<int>("Floordata Index", [&](auto&& sector) { return sector.floordata_index(); });
            sector_filters.add_getter<int>("Material", [&](auto&& sector) { return sector.material(); });
            sector_filters.add_getter<int>("Box Index", [&](auto&& sector) { return sector.box_index(); });
            sector_filters.add_getter<bool>("Stopper", [&](auto&& sector) { return sector.stopper(); });
            sector_filters.add_getter<int>("Room Above", [&](auto&& sector) { return sector.room_above(); });
            sector_filters.add_getter<int>("Floor", [&](auto&& sector) { return sector.floor(); });
            sector_filters.add_getter<int>("Room Below", [&](auto&& sector) { return sector.room_below(); });
            sector_filters.add_getter<int>("Ceiling", [&](auto&& sector) { return sector.ceiling(); });
            return sector_filters;
        }
        
        return std::nullopt;
    }

    void RoomsWindow::apply_sector_filters()
    {
        if (auto room_ptr = _selected_room.lock())
        {
            // Temporary experiment for sector highlighting on the map renderer.
            // Re-test the room against floordata only filters - if it matches
            // pass the results through to the map renderer for highlighting.
            if (auto sector_filters = convert_to_sector_filters())
            {
                _map_renderer->set_selection(room_ptr->sectors()
                    | std::views::filter([&](auto&& s) { return sector_filters->match(*s); })
                    | std::ranges::to<std::vector>());
            }
            else
            {
                _map_renderer->set_selection(std::vector<std::shared_ptr<ISector>>{});
            }
        }
        else
        {
            _map_renderer->set_selection(std::vector<std::shared_ptr<ISector>>{});
        }
    }

    std::string RoomsWindow::type() const
    {
        return "Rooms";
    }

    std::string RoomsWindow::title() const
    {
        return _id;
    }
}