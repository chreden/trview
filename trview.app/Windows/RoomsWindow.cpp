#include "RoomsWindow.h"
#include <trview.app/Elements/IRoom.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        void add_room_flags(IClipboard& clipboard, trlevel::LevelVersion version, const IRoom& room)
        {
            using trlevel::LevelVersion;
            const auto add_flag = [&](const std::wstring& name, bool flag) 
            {
                auto value = to_utf8(format_bool(flag));
                ImGui::TableNextColumn();
                if (ImGui::Selectable(to_utf8(name).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                {
                    clipboard.write(to_utf16(value));
                }
                ImGui::TableNextColumn();
                ImGui::Text(value.c_str());
            };
            const auto add_flag_min = [&](LevelVersion min_version, const std::wstring& name, const std::wstring& alt_name, bool flag)
            {
                add_flag(version >= min_version ? name : alt_name, flag);
            };

            add_flag(L"Water", room.water());
            add_flag(L"Bit 1", room.flag(IRoom::Flag::Bit1));
            add_flag(L"Bit 2", room.flag(IRoom::Flag::Bit2));
            add_flag_min(LevelVersion::Tomb2, L"Outside / 3", L"Bit 3", room.outside());
            add_flag(L"Bit 4", room.flag(IRoom::Flag::Bit4));
            add_flag_min(LevelVersion::Tomb2, L"Wind / 5", L"Bit 5", room.flag(IRoom::Flag::Wind));
            add_flag(L"Bit 6", room.flag(IRoom::Flag::Bit6));
            if (version == LevelVersion::Tomb3)
            {
                add_flag(L"Quicksand / 7", room.flag(IRoom::Flag::Quicksand));
            }
            else if (version > LevelVersion::Tomb3)
            {
                add_flag(L"Block Lens Flare / 7", room.flag(IRoom::Flag::NoLensFlare));
            }
            else
            {
                add_flag(L"Bit 7", room.flag(IRoom::Flag::Bit7));
            }
            add_flag_min(LevelVersion::Tomb3, L"Caustics / 8", L"Bit 8", room.flag(IRoom::Flag::Caustics));
            add_flag_min(LevelVersion::Tomb3, L"Reflectivity / 9", L"Bit 9", room.flag(IRoom::Flag::WaterReflectivity));
            add_flag_min(LevelVersion::Tomb4, L"Snow (NGLE) / 10", L"Bit 10", room.flag(IRoom::Flag::Bit10));
            add_flag_min(LevelVersion::Tomb4, L"D / Rain / 11", L"Bit 11", room.flag(IRoom::Flag::Bit11));
            add_flag_min(LevelVersion::Tomb4, L"P / Cold / 12", L"Bit 12", room.flag(IRoom::Flag::Bit12));
            add_flag(L"Bit 13", room.flag(IRoom::Flag::Bit13));
            add_flag(L"Bit 14", room.flag(IRoom::Flag::Bit14));
            add_flag(L"Bit 15", room.flag(IRoom::Flag::Bit15));
        }
    }

    const std::string RoomsWindow::Names::sync_room{ "sync_room" };
    const std::string RoomsWindow::Names::track_item{ "track_item" };
    const std::string RoomsWindow::Names::track_trigger{ "track_trigger" };
    const std::string RoomsWindow::Names::rooms_listbox{ "Rooms" };
    const std::string RoomsWindow::Names::triggers_listbox{ "Triggers" };
    const std::string RoomsWindow::Names::stats_listbox{ "Stats" };
    const std::string RoomsWindow::Names::minimap{ "minimap" };
    const std::string RoomsWindow::Names::neighbours_listbox{ "neighbours" };
    const std::string RoomsWindow::Names::items_listbox{ "items" };

    RoomsWindow::RoomsWindow(const IMapRenderer::Source& map_renderer_source, const std::shared_ptr<IClipboard>& clipboard)
        : _map_renderer(map_renderer_source(Size(341, 341))), _clipboard(clipboard)
    {
        static int number = 0;
        _id = "Rooms " + std::to_string(++number);

        _map_renderer->set_render_mode(IMapRenderer::RenderMode::Texture);
        _token_store += _map_renderer->on_sector_hover += [this](const std::shared_ptr<ISector>& sector)
        {
            if (!sector)
            {
                _map_tooltip.set_visible(false);
                return;
            }

            std::wstring text;
            if (sector->flags() & SectorFlag::RoomAbove)
            {
                text += L"Above: " + std::to_wstring(sector->room_above());
            }
            if (sector->flags() & SectorFlag::RoomBelow)
            {
                text += ((sector->flags() & SectorFlag::RoomAbove) ? L", " : L"") +
                    std::wstring(L"Below: ") + std::to_wstring(sector->room_below());
            }
            _map_tooltip.set_text(text);
            _map_tooltip.set_visible(!text.empty());
        };
    }

    void RoomsWindow::set_current_room(uint32_t room)
    {
        if (room == _current_room)
        {
            return;
        }

        _current_room = room;
        _scroll_to_room = true;
        if (_sync_room && _current_room < _all_rooms.size())
        {
            _selected_room = _current_room;
            const auto room = std::find_if(_all_rooms.begin(), _all_rooms.end(),
                [&](auto&& r) 
                { 
                    const auto locked = r.lock();
                    return locked && locked->number() == _selected_room; 
                });
            if (room != _all_rooms.end())
            {
                _map_renderer->load(room->lock());
            }
        }
    }

    void RoomsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    void RoomsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void RoomsWindow::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
        _current_room = 0xffffffff;
    }

    void RoomsWindow::set_selected_item(const Item& item)
    {
        _selected_item = item;
    }

    void RoomsWindow::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
    }

    void RoomsWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _selected_trigger.reset();
        _all_triggers = triggers;
    }

    void RoomsWindow::set_sync_room(bool value)
    {
        if (_sync_room != value)
        {
            _sync_room = value;
            set_current_room(_current_room);
        }
    }

    void RoomsWindow::set_track_item(bool value)
    {
        if (_track_item != value)
        {
            _track_item = value;
            if (_selected_item.has_value())
            {
                set_selected_item(_selected_item.value());
            }
        }
    }

    void RoomsWindow::set_track_trigger(bool value)
    {
        if (_track_trigger != value)
        {
            _track_trigger = value;
            set_selected_trigger(_selected_trigger);
        }
    }

    void RoomsWindow::render(bool vsync)
    {
        if (!render_rooms_window())
        {
            on_window_closed();
            return;
        }
    }

    bool RoomsWindow::render_rooms_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(620, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_rooms_list();
            ImGui::SameLine();
            render_room_details();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        _map_tooltip.render();
        return stay_open;
    }

    void RoomsWindow::render_rooms_list()
    {
        if (ImGui::BeginChild("Rooms List", ImVec2(270, 0), true))
        {
            bool sync_room = _sync_room;
            if (ImGui::Checkbox("Sync Room##syncroom", &sync_room))
            {
                set_sync_room(sync_room);
            }
            ImGui::SameLine();
            bool track_item = _track_item;
            if (ImGui::Checkbox("Track Item##trackitem", &track_item))
            {
                set_track_item(track_item);
            }
            ImGui::SameLine();
            bool track_trigger = _track_trigger;
            if (ImGui::Checkbox("Track Triger##tracktrigger", &track_trigger))
            {
                set_track_trigger(track_trigger);
            }

            if (ImGui::BeginTable("##roomslist", 3, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Items");
                ImGui::TableSetupColumn("Triggers");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                auto item_count = [&](const IRoom& room)
                {
                    return std::count_if(_all_items.begin(), _all_items.end(), [&room](const auto& item) { return item.room() == room.number(); });
                };

                auto trigger_count = [&](const IRoom& room)
                {
                    return std::count_if(_all_triggers.begin(), _all_triggers.end(), [&room](const auto& trigger) { return trigger.lock()->room() == room.number(); });
                };

                auto specs = ImGui::TableGetSortSpecs();
                if (specs && specs->SpecsDirty)
                {
                    std::sort(_all_rooms.begin(), _all_rooms.end(),
                        [&](const auto& l, const auto& r) -> int
                        {
                            const auto l_l = l.lock();
                            const auto r_l = r.lock();

                            switch (specs->Specs[0].ColumnIndex)
                            {
                            case 0:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l_l->number() < r_l->number()) : (l_l->number() > r_l->number());
                            case 1:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (item_count(*l_l) < item_count(*r_l)) : (item_count(*l_l) > item_count(*r_l));
                            case 2:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (trigger_count(*l_l) < trigger_count(*r_l)) : (trigger_count(*l_l) > trigger_count(*r_l));
                            }
                            return 0;
                        });
                    specs->SpecsDirty = false;
                }

                for (const auto& room : _all_rooms)
                {
                    auto room_ptr = room.lock();

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = room_ptr->number() == _selected_room;
                    if (selected && _scroll_to_room)
                    {
                        const auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
                        if (!ImGui::IsRectVisible(pos, pos + ImVec2(1, 1)))
                        {
                            ImGui::SetScrollHereY();
                        }
                        _scroll_to_room = false;
                    }
                    if (ImGui::Selectable((std::to_string(room_ptr->number()) + std::string("##") + std::to_string(room_ptr->number())).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                    {
                        _selected_room = room_ptr->number();
                        _map_renderer->load(room_ptr);
                        if (_sync_room)
                        {
                            on_room_selected(_selected_room);
                        }
                    }
                    ImGui::SetItemAllowOverlap();
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item_count(*room_ptr)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(trigger_count(*room_ptr)).c_str());
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    void RoomsWindow::render_room_details()
    {
        auto current_room = [&]() -> std::weak_ptr<IRoom>
        {
            for (const auto& room : _all_rooms)
            {
                if (room.lock()->number() == _selected_room)
                {
                    return room;
                }
            }
            return {};
        };


        if (ImGui::BeginChild("Room Details", ImVec2(), true))
        {
            if (_selected_room < _all_rooms.size())
            {
                auto room = current_room().lock();
                if (room)
                {
                    if (_map_renderer->loaded())
                    {
                        _map_renderer->render();
                        auto texture = _map_renderer->texture();
                        if (!texture.has_content())
                        {
                            return;
                        }
                        _map_renderer->set_window_size(texture.size());

                        float remainder = (341 - texture.size().height) * 0.5f;
                        ImGui::SetCursorPosX(std::round((ImGui::GetWindowSize().x - texture.size().width) * 0.5f));
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + std::floor(remainder));
                        
                        const auto io = ImGui::GetIO();
                        if (io.WantCaptureMouse)
                        {
                            const auto mouse_pos = io.MousePos;
                            auto top_left = ImGui::GetCursorScreenPos();
                            auto adjusted = mouse_pos - top_left;
                            _map_renderer->set_cursor_position(Point(adjusted.x, adjusted.y));

                            auto sector = _map_renderer->sector_at_cursor();
                            if (sector)
                            {
                                if (io.MouseClicked[0])
                                {
                                    if (sector->flags() & SectorFlag::Portal)
                                    {
                                        on_room_selected(sector->portal());
                                    }
                                    else if (sector->room_below() != 0xff)
                                    {
                                        on_room_selected(sector->room_below());
                                    }
                                    else
                                    {
                                        // Select triggers
                                        for (const auto& trigger : _all_triggers)
                                        {
                                            auto trigger_ptr = trigger.lock();
                                            if (trigger_ptr && trigger_ptr->room() == _current_room && trigger_ptr->sector_id() == sector->id())
                                            {
                                                on_trigger_selected(trigger);
                                                break;
                                            }
                                        }
                                    }
                                }
                                else if (io.MouseClicked[1])
                                {
                                    if (sector->room_above() != 0xff)
                                    {
                                        on_room_selected(sector->room_above());
                                    }
                                }
                            }
                        }
                        ImGui::Image(texture.view().Get(), ImVec2(texture.size().width, texture.size().height));
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + std::ceil(remainder));
                    }

                    auto add_stat = [&](const std::string& name, const std::string& value)
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _clipboard->write(to_utf16(value));
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(value.c_str());
                    };

                    ImGui::Text("Room Details");
                    if (ImGui::BeginTable("Room Details", 2, ImGuiTableFlags_ScrollY, ImVec2(0, 150)))
                    {
                        ImGui::TableSetupColumn("Name");
                        ImGui::TableSetupColumn("Value");
                        ImGui::TableNextRow();

                        add_stat("X", std::to_string(room->info().x));
                        add_stat("Y", std::to_string(room->info().yBottom));
                        add_stat("Z", std::to_string(room->info().z));
                        if (room->alternate_mode() != Room::AlternateMode::None)
                        {
                            add_stat("Alternate", std::to_string(room->alternate_room()));
                            if (room->alternate_group() != 0xff)
                            {
                                add_stat("Alternate Group", std::to_string(room->alternate_group()));
                            }
                        }
                        add_room_flags(*_clipboard, _level_version, *room);
                        ImGui::EndTable();
                    }

                    ImGui::Text("Items");
                    if (ImGui::BeginTable("Items", 2, ImGuiTableFlags_ScrollY, ImVec2(0, 150)))
                    {
                        ImGui::TableSetupColumn("#");
                        ImGui::TableSetupColumn("Type");
                        ImGui::TableSetupScrollFreeze(1, 1);
                        ImGui::TableHeadersRow();
                        
                        for (const auto& item : _all_items)
                        {
                            if (item.room() == room->number())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text(std::to_string(item.number()).c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(to_utf8(item.type()).c_str());
                            }
                        }

                        ImGui::EndTable();
                    }

                    ImGui::Text("Neighbours");
                    if (ImGui::BeginTable("Neighbours", 1, ImGuiTableFlags_ScrollY, ImVec2(0, 150)))
                    {
                        ImGui::TableSetupColumn("#");

                        for (auto& neighbour : room->neighbours())
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text(std::to_string(neighbour).c_str());
                        }

                        ImGui::EndTable();
                    }

                    ImGui::Text("Triggers");
                    if (ImGui::BeginTable("Triggers", 2, ImGuiTableFlags_ScrollY, ImVec2(0, 150)))
                    {
                        ImGui::TableSetupColumn("#");
                        ImGui::TableSetupColumn("Type");
                        ImGui::TableSetupScrollFreeze(1, 1);
                        ImGui::TableHeadersRow();

                        for (const auto& trigger : _all_triggers)
                        {
                            const auto trigger_ptr = trigger.lock();
                            if (trigger_ptr->room() == room->number())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text(std::to_string(trigger_ptr->number()).c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(to_utf8(trigger_type_name(trigger_ptr->type())).c_str());
                            }
                        }

                        ImGui::EndTable();
                    }
                }
            }
        }
        ImGui::EndChild();
    }

    void RoomsWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
    }

    void RoomsWindow::update(float delta)
    {
    }
}