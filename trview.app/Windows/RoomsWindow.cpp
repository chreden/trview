#include "RoomsWindow.h"
#include <trview.ui/Image.h>
#include <trview.app/Elements/IRoom.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.common/Strings.h>
#include <trview.input/IMouse.h>
#include "../Resources/resource.h"

namespace trview
{
    namespace
    {
        ui::Listbox::Item create_listbox_item(const std::weak_ptr<IRoom>& room_ptr, const std::vector<Item>& items, const std::vector<std::weak_ptr<ITrigger>>& triggers)
        {
            const auto room = room_ptr.lock();
            if (!room)
            {
                return {{}};
            }

            auto item_count = std::count_if(items.begin(), items.end(), [&room](const auto& item) { return item.room() == room->number(); });
            auto trigger_count = std::count_if(triggers.begin(), triggers.end(), [&room](const auto& trigger) 
                { 
                    auto trigger_ptr = trigger.lock();
                    return trigger_ptr && trigger_ptr->room() == room->number();
                });

            return
            {
                {
                    { L"#", std::to_wstring(room->number()) },
                    { L"Items", std::to_wstring(item_count) },
                    { L"Triggers", std::to_wstring(trigger_count) }
                }
            };
        }

        ui::Listbox::Item create_listbox_item(const Item& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"ID", std::to_wstring(item.type_id()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", item.type() }} };
        }

        ui::Listbox::Item create_listbox_item(const ITrigger& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Type", trigger_type_name(item.type()) }} };
        }

        ui::Listbox::Item make_item (const std::wstring& name, const std::wstring& value)
        {
            return ui::Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };

        void add_room_flags(std::vector<ui::Listbox::Item>& stats, trlevel::LevelVersion version, const IRoom& room)
        {
            using trlevel::LevelVersion;
            const auto add_flag = [&](const std::wstring& name, bool flag) { stats.push_back(make_item(name, format_bool(flag))); };
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

    RoomsWindow::RoomsWindow(const graphics::IDeviceWindow::Source& device_window_source,
        const ui::render::IRenderer::Source& renderer_source,
        const ui::render::IMapRenderer::Source& map_renderer_source,
        const ui::IInput::Source& input_source,
        const std::shared_ptr<IClipboard>& clipboard,
        const IBubble::Source& bubble_source,
        const Window& parent,
        const ui::UiSource& ui_source)
        : CollapsiblePanel(device_window_source, renderer_source(Size(630, 680)), parent, L"trview.rooms", L"Rooms", input_source, Size(630, 680)), _map_renderer(map_renderer_source(Size(341, 341))),
        _bubble(bubble_source(*_ui)), _clipboard(clipboard)
    {
        CollapsiblePanel::on_window_closed += IRoomsWindow::on_window_closed;

        set_panels(create_left_panel(ui_source), create_right_panel(ui_source));
        set_allow_increase_height(false);

        using namespace input;
        using namespace ui;

        _token_store += _input->mouse()->mouse_click += [&](IMouse::Button button)
        {
            auto sector = _map_renderer->sector_at_cursor();
            if (!sector)
            {
                return;
            }

            if (button == IMouse::Button::Left)
            {
                if (sector->flags() & SectorFlag::Portal)
                {
                    load_room_details(_all_rooms[sector->portal()]);
                    on_room_selected(sector->portal());
                    return;
                }

                if (sector->room_below() != 0xff)
                {
                    load_room_details(_all_rooms[sector->room_below()]);
                    on_room_selected(sector->room_below());
                    return;
                }

                // Select triggers
                for (const auto& trigger : _all_triggers)
                {
                    auto trigger_ptr = trigger.lock();
                    if (trigger_ptr && trigger_ptr->room() == _current_room && trigger_ptr->sector_id() == sector->id())
                    {
                        _triggers_list->set_selected_item(create_listbox_item(*trigger_ptr));
                        on_trigger_selected(trigger);
                        break;
                    }
                }
            }
            else if (button == IMouse::Button::Right)
            {
                if (sector->room_above() != 0xff)
                {
                    load_room_details(_all_rooms[sector->room_above()]);
                    on_room_selected(sector->room_above());
                    return;
                }
            }
        };

        _token_store += _input->mouse()->mouse_move += [&](long, long)
        {
            // Only do work if we have actually loaded a map.
            if (_map_renderer->loaded())
            {
                // Adjust mouse coordinates to be relative to what the map renderer thinks is going on.
                auto ccp = client_cursor_position(window());
                _map_renderer->set_cursor_position(ccp - _minimap->absolute_position() + _map_renderer->first());
                if (_map_tooltip && _map_tooltip->visible())
                {
                    _map_tooltip->set_position(client_cursor_position(window()) - _minimap->parent()->absolute_position());
                }
                render_minimap();
            }
        };

        _token_store += _map_renderer->on_sector_hover += [this](const std::shared_ptr<ISector>& sector)
        {
            if (!sector)
            {
                _map_tooltip->set_visible(false);
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
            _map_tooltip->set_text(text);
            _map_tooltip->set_position(client_cursor_position(window()) - _minimap->parent()->absolute_position());
            _map_tooltip->set_visible(!text.empty());
        };
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_left_panel(const ui::UiSource& ui_source)
    {
        using namespace ui;

        auto left_panel = ui_source(IDR_UI_ROOMS_WINDOW_LEFT_PANEL);
        _sync_room_checkbox = left_panel->find<Checkbox>(Names::sync_room);
        _token_store += _sync_room_checkbox->on_state_changed += [this](bool value)
        {
            set_sync_room(value);
        };

        _track_item_checkbox = left_panel->find<Checkbox>(Names::track_item);
        _track_item_checkbox->set_state(false);
        _token_store += _track_item_checkbox->on_state_changed += [this](bool value)
        {
            set_track_item(value);
        };

        _track_trigger_checkbox = left_panel->find<Checkbox>(Names::track_trigger);
        _track_trigger_checkbox->set_state(false);
        _token_store += _track_trigger_checkbox->on_state_changed += [this](bool value)
        {
            set_track_trigger(value);
        };

        _rooms_list = left_panel->find<Listbox>(Names::rooms_listbox);
        _token_store += _rooms_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_room_details(_all_rooms[index]);
            if (_sync_room)
            {
                on_room_selected(index);
            }
        };

        // Fix items list size now that it has been added to the panel.
        _rooms_list->set_size(Size(250, left_panel->size().height - _rooms_list->position().y));

        return left_panel;
    }

    void RoomsWindow::set_current_room(uint32_t room)
    {
        if (room == _current_room)
        {
            return;
        }

        _current_room = room;
        if (_sync_room && _current_room < _all_rooms.size())
        {
            auto item = create_listbox_item(_all_rooms[room], _all_items, _all_triggers);
            _rooms_list->set_selected_item(item);

            load_room_details(_all_rooms[_current_room]);
        }
    }

    void RoomsWindow::set_items(const std::vector<Item>& items)
    {
        using namespace ui;
        _items_list->set_items({});
        _all_items = items;
    }

    void RoomsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void RoomsWindow::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(rooms.begin(), rooms.end(), std::back_inserter(list_items), [&](auto&& room) { return create_listbox_item(room, _all_items, _all_triggers); });
        _rooms_list->set_items(list_items);
        _all_rooms = rooms;
        _current_room = 0xffffffff;
    }

    void RoomsWindow::set_selected_item(const Item& item)
    {
        _selected_item = item;
        if (_track_item)
        {
            load_room_details(_all_rooms[item.room()]);
            const auto& list_item = create_listbox_item(item);
            _items_list->set_selected_item(list_item);
        }
    }

    void RoomsWindow::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        if (_track_trigger)
        {
            if (auto trigger_ptr = trigger.lock())
            {
                load_room_details(_all_rooms[trigger_ptr->room()]);
                const auto& list_item = create_listbox_item(*trigger_ptr);
                _triggers_list->set_selected_item(list_item);
            }
        }
    }

    void RoomsWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _selected_trigger.reset();
        _triggers_list->set_items({});
        _all_triggers = triggers;
    }

    void RoomsWindow::render_minimap()
    {
        _map_renderer->render(false);
        auto texture = _map_renderer->texture();
        if (!texture.has_content())
        {
            return;
        }
        auto map_size = texture.size();
        _minimap->set_texture(_map_renderer->texture());
        _minimap->set_size(map_size);
        _minimap->set_position(Point((341 - map_size.width) / 2.0f, (341 - map_size.height) / 2.0f));
    }

    void RoomsWindow::load_room_details(const std::weak_ptr<IRoom>& room_ptr)
    {
        const auto room = room_ptr.lock();
        if (!room)
        {
            return;
        }

        using namespace ui;

        // Clear lists.
        _items_list->clear_selection();
        _triggers_list->clear_selection();
        _neighbours_list->clear_selection();

        // Minimap stuff 
        _map_renderer->load(room);
        render_minimap();

        // Load the stats for the room.
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"X", std::to_wstring(room->info().x)));
        stats.push_back(make_item(L"Y", std::to_wstring(room->info().yBottom)));
        stats.push_back(make_item(L"Z", std::to_wstring(room->info().z)));
        if (room->alternate_mode() != Room::AlternateMode::None)
        {
            stats.push_back(make_item(L"Alternate", std::to_wstring(room->alternate_room())));
            if (room->alternate_group() != 0xff)
            {
                stats.push_back(make_item(L"Alternate Group", std::to_wstring(room->alternate_group())));
            }
        }
        add_room_flags(stats, _level_version, *room);
        _stats_box->set_items(stats);

        std::vector<Listbox::Item> list_neighbours;
        for (auto& neighbour : room->neighbours())
        {
            list_neighbours.push_back({{{{ L"#", std::to_wstring(neighbour)}}}});
        }
        _neighbours_list->set_items(list_neighbours);

        // Contents of the room.
        std::vector<Listbox::Item> list_items;
        for (const auto& item : _all_items)
        {
            if (item.room() == room->number())
            {
                list_items.push_back(create_listbox_item(item));
            }
        }
        _items_list->set_items(list_items);

        std::vector<Listbox::Item> list_triggers;
        for (const auto& trigger : _all_triggers)
        {
            auto trigger_ptr = trigger.lock();
            if (trigger_ptr && trigger_ptr->room() == room->number())
            {
                list_triggers.push_back(create_listbox_item(*trigger_ptr));
            }
        }
        _triggers_list->set_items(list_triggers);
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_right_panel(const ui::UiSource& ui_source)
    {
        using namespace ui;

        const float panel_width = 380;
        const float upper_height = 380;
        auto right_panel = ui_source(IDR_UI_ROOMS_WINDOW_RIGHT_PANEL);
        
        _minimap = right_panel->find<ui::Image>(Names::minimap);
        _map_tooltip = std::make_unique<Tooltip>(*_minimap->parent());

        _stats_box = right_panel->find<Listbox>(Names::stats_listbox);
        _token_store += _stats_box->on_item_selected += [this](const auto& item)
        {
            if (item.value(L"Name") == L"Alternate")
            {
                on_room_selected(std::stoi(item.value(L"Value")));
            }
            else
            {
                _clipboard->write(window(), item.value(L"Value"));
                _bubble->show(client_cursor_position(window()) - Point(0, 20));
            }
        };

        _neighbours_list = right_panel->find<Listbox>(Names::neighbours_listbox);
        _token_store += _neighbours_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_room_details(_all_rooms[index]);
            on_room_selected(index);
        };
        
        _items_list = right_panel->find<Listbox>(Names::items_listbox);
        _token_store += _items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            on_item_selected(_all_items[index]);
        };

        _triggers_list = right_panel->find<Listbox>(Names::triggers_listbox);
        _token_store += _triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            on_trigger_selected(_all_triggers[index]);
        };
        
        return right_panel;
    }

    void RoomsWindow::set_sync_room(bool value)
    {
        if (_sync_room != value)
        {
            _sync_room = value;
            set_current_room(_current_room);
        }

        if (_sync_room_checkbox->state() != _sync_room)
        {
            _sync_room_checkbox->set_state(_sync_room);
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

        if (_track_item_checkbox->state() != _track_item)
        {
            _track_item_checkbox->set_state(_track_item);
        }
    }

    void RoomsWindow::set_track_trigger(bool value)
    {
        if (_track_trigger != value)
        {
            _track_trigger = value;
            set_selected_trigger(_selected_trigger);
        }

        if (_track_trigger_checkbox->state() != _track_trigger)
        {
            _track_trigger_checkbox->set_state(_track_trigger);
        }
    }

    void RoomsWindow::render(bool vsync)
    {
        CollapsiblePanel::render(vsync);
    }

    void RoomsWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
        _triggers_list->clear_selection();
    }

    void RoomsWindow::update(float delta)
    {
        _ui->update(delta);
    }
}