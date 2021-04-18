#include "RoomsWindow.h"
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Image.h>
#include <trview.app/Elements/Room.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>
#include <trview.common/Strings.h>
#include <trview.input/IMouse.h>

namespace trview
{
    namespace
    {
        namespace Colours
        {
            const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel{ 1.0f, 0.25f, 0.25f, 0.25f };
            const Colour ItemDetails{ 1.0f, 0.225f, 0.225f, 0.225f };
            const Colour Triggers{ 1.0f, 0.20f, 0.20f, 0.20f };
            const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        ui::Listbox::Item create_listbox_item(const IRoom* const room, const std::vector<Item>& items, const std::vector<Trigger*>& triggers)
        {
            auto item_count = std::count_if(items.begin(), items.end(), [&room](const auto& item) { return item.room() == room->number(); });
            auto trigger_count = std::count_if(triggers.begin(), triggers.end(), [&room](const auto& trigger) { return trigger->room() == room->number(); });

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

        ui::Listbox::Item create_listbox_item(const Trigger& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Type", trigger_type_name(item.type()) }} };
        }
    }

    const std::string RoomsWindow::Names::rooms_listbox{ "Rooms" };
    const std::string RoomsWindow::Names::triggers_listbox{ "Triggers" };

    RoomsWindow::RoomsWindow(const graphics::IDeviceWindow::Source& device_window_source,
        const ui::render::IRenderer::Source& renderer_source,
        const ui::render::IMapRenderer::Source& map_renderer_source,
        const ui::IInput::Source& input_source,
        const Window& parent)
        : CollapsiblePanel(device_window_source, renderer_source(Size(630, 680)), parent, L"trview.rooms", L"Rooms", input_source, Size(630, 680)), _map_renderer(map_renderer_source(Size(341, 341)))
    {
        CollapsiblePanel::on_window_closed += IRoomsWindow::on_window_closed;

        set_panels(create_left_panel(), create_right_panel());
        set_allow_increase_height(false);

        using namespace input;
        using namespace ui;

        _token_store += _input->mouse().mouse_click += [&](IMouse::Button button)
        {
            auto sector = _map_renderer->sector_at_cursor();
            if (!sector)
            {
                return;
            }

            if (button == IMouse::Button::Left)
            {
                if (sector->flags & SectorFlag::Portal)
                {
                    load_room_details(*_all_rooms[sector->portal()]);
                    on_room_selected(sector->portal());
                    return;
                }

                if (sector->room_below() != 0xff)
                {
                    load_room_details(*_all_rooms[sector->room_below()]);
                    on_room_selected(sector->room_below());
                    return;
                }

                // Select triggers
                for (const auto& trigger : _all_triggers)
                {
                    if (trigger->room() == _current_room && trigger->sector_id() == sector->id())
                    {
                        _triggers_list->set_selected_item(create_listbox_item(*trigger));
                        on_trigger_selected(trigger);
                        break;
                    }
                }
            }
            else if (button == IMouse::Button::Right)
            {
                if (sector->room_above() != 0xff)
                {
                    load_room_details(*_all_rooms[sector->room_above()]);
                    on_room_selected(sector->room_above());
                    return;
                }
            }
        };

        _token_store += _input->mouse().mouse_move += [&](long, long)
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

        _token_store += _map_renderer->on_sector_hover += [this](const std::shared_ptr<Sector>& sector)
        {
            if (!sector)
            {
                _map_tooltip->set_visible(false);
                return;
            }

            std::wstring text;
            if (sector->flags & SectorFlag::RoomAbove)
            {
                text += L"Above: " + std::to_wstring(sector->room_above());
            }
            if (sector->flags & SectorFlag::RoomBelow)
            {
                text += ((sector->flags & SectorFlag::RoomAbove) ? L", " : L"") +
                    std::wstring(L"Below: ") + std::to_wstring(sector->room_below());
            }
            _map_tooltip->set_text(text);
            _map_tooltip->set_position(client_cursor_position(window()) - _minimap->parent()->absolute_position());
            _map_tooltip->set_visible(!text.empty());
        };
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_left_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<StackPanel>(Size(250, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto controls = std::make_unique<StackPanel>(Size(250, 20), Colours::LeftPanel, Size(6, 2), StackPanel::Direction::Horizontal, SizeMode::Manual);
        controls->set_margin(Size(2, 2));

        _sync_room_checkbox = controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Sync Room"));
        _sync_room_checkbox->set_state(true);
        _token_store += _sync_room_checkbox->on_state_changed += [this](bool value)
        {
            set_sync_room(value);
        };

        _track_item_checkbox = controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Track Item"));
        _track_item_checkbox->set_state(false);
        _token_store += _track_item_checkbox->on_state_changed += [this](bool value)
        {
            set_track_item(value);
        };

        _track_trigger_checkbox = controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Track Trigger"));
        _track_trigger_checkbox->set_state(false);
        _token_store += _track_trigger_checkbox->on_state_changed += [this](bool value)
        {
            set_track_trigger(value);
        };

        _controls = left_panel->add_child(std::move(controls));
        _rooms_list = left_panel->add_child(create_rooms_list());

        // Fix items list size now that it has been added to the panel.
        _rooms_list->set_size(Size(250, left_panel->size().height - _rooms_list->position().y));

        return left_panel;
    }

    std::unique_ptr<ui::Listbox> RoomsWindow::create_rooms_list()
    {
        using namespace ui;

        auto rooms_list = std::make_unique<Listbox>(Size(250, window().size().height - _controls->size().height), Colours::LeftPanel);
        rooms_list->set_name(Names::rooms_listbox);
        rooms_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 40 },
                { Listbox::Column::Type::Number, L"Items", 100 },
                { Listbox::Column::Type::Number, L"Triggers", 100 }
            }
        );
        _token_store += rooms_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_room_details(*_all_rooms[index]);
            if (_sync_room)
            {
                on_room_selected(index);
            }
        };
        return rooms_list;
    }

    void RoomsWindow::create_neighbours_list(ui::Control& parent)
    {
        using namespace ui;

        auto group_box = std::make_unique<GroupBox>(Size(190, 140), Colours::ItemDetails, Colours::DetailsBorder, L"Neighbours");
        auto neighbours_list = std::make_unique<Listbox>(Size(180, 140 - 21), Colours::LeftPanel);
        neighbours_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 170 }
            }
        );
        _token_store += neighbours_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_room_details(*_all_rooms[index]);
            on_room_selected(index);
        };
        _neighbours_list = group_box->add_child(std::move(neighbours_list));
        parent.add_child(std::move(group_box));
    }

    void RoomsWindow::create_items_list(ui::Control& parent)
    {
        using namespace ui;

        auto group_box = std::make_unique<GroupBox>(Size(190, 150), Colours::ItemDetails, Colours::DetailsBorder, L"Items");
        auto items_list = std::make_unique<Listbox>(Size(180, 150 - 21), Colours::LeftPanel);
        items_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::String, L"Type", 140 }
            }
        );
        _token_store += items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            on_item_selected(_all_items[index]);
        };
        _items_list = group_box->add_child(std::move(items_list));
        parent.add_child(std::move(group_box));
    }

    void RoomsWindow::create_triggers_list(ui::Control& parent)
    {
        using namespace ui;

        auto group_box = std::make_unique<GroupBox>(Size(190, 140), Colours::ItemDetails, Colours::DetailsBorder, L"Triggers");
        auto triggers_list = std::make_unique<Listbox>(Size(180, 140 - 21), Colours::LeftPanel);
        triggers_list->set_name(Names::triggers_listbox);
        triggers_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::String, L"Type", 140 }
            }
        );
        _token_store += triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            on_trigger_selected(_all_triggers[index]);
        };
        _triggers_list = group_box->add_child(std::move(triggers_list));
        parent.add_child(std::move(group_box));
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

            load_room_details(*_all_rooms[_current_room]);
        }
    }

    void RoomsWindow::set_items(const std::vector<Item>& items)
    {
        using namespace ui;
        _items_list->set_items({});
        _all_items = items;
    }

    void RoomsWindow::set_rooms(const std::vector<IRoom*>& rooms)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(rooms.begin(), rooms.end(), std::back_inserter(list_items), [&](const auto& room) { return create_listbox_item(room, _all_items, _all_triggers); });
        _rooms_list->set_items(list_items);
        _all_rooms = rooms;
        _current_room = 0xffffffff;
    }

    void RoomsWindow::set_selected_item(const Item& item)
    {
        _selected_item = item;
        if (_track_item)
        {
            load_room_details(*_all_rooms[item.room()]);
            const auto& list_item = create_listbox_item(item);
            _items_list->set_selected_item(list_item);
        }
    }

    void RoomsWindow::set_selected_trigger(const Trigger* const trigger)
    {
        _selected_trigger = trigger;
        if (_track_trigger)
        {
            load_room_details(*_all_rooms[trigger->room()]);
            const auto& list_item = create_listbox_item(*trigger);
            _triggers_list->set_selected_item(list_item);
        }
    }

    void RoomsWindow::set_triggers(const std::vector<Trigger*>& triggers)
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

    void RoomsWindow::load_room_details(const IRoom& room)
    {
        using namespace ui;

        // Clear lists.
        _items_list->clear_selection();
        _triggers_list->clear_selection();
        _neighbours_list->clear_selection();

        // Minimap stuff 
        _map_renderer->load(&room);
        render_minimap();

        // Load the stats for the room.
        auto make_item = [](const auto& name, const auto& value)
        {
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };

        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"X", std::to_wstring(room.info().x)));
        stats.push_back(make_item(L"Y", std::to_wstring(room.info().yBottom)));
        stats.push_back(make_item(L"Z", std::to_wstring(room.info().z)));
        if (room.water())
        {
            stats.push_back(make_item(L"Water", format_bool(room.water())));
        }
        if (room.outside())
        {
            stats.push_back(make_item(L"Outside", format_bool(room.outside())));
        }
        if (room.quicksand())
        {
            stats.push_back(make_item(L"Quicksand", format_bool(room.quicksand())));
        }
        if (room.alternate_mode() != Room::AlternateMode::None)
        {
            stats.push_back(make_item(L"Alternate", std::to_wstring(room.alternate_room())));
            if (room.alternate_group() != 0xff)
            {
                stats.push_back(make_item(L"Alternate Group", std::to_wstring(room.alternate_group())));
            }
        }
        _stats_box->set_items(stats);

        std::vector<Listbox::Item> list_neighbours;
        for (auto& neighbour : room.neighbours())
        {
            list_neighbours.push_back({{{{ L"#", std::to_wstring(neighbour)}}}});
        }
        _neighbours_list->set_items(list_neighbours);

        // Contents of the room.
        std::vector<Listbox::Item> list_items;
        for (const auto& item : _all_items)
        {
            if (item.room() == room.number())
            {
                list_items.push_back(create_listbox_item(item));
            }
        }
        _items_list->set_items(list_items);

        std::vector<Listbox::Item> list_triggers;
        for (const auto& trigger : _all_triggers)
        {
            if (trigger->room() == room.number())
            {
                list_triggers.push_back(create_listbox_item(*trigger));
            }
        }
        _triggers_list->set_items(list_triggers);
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 380;
        const float upper_height = 380;
        auto right_panel = std::make_unique<StackPanel>(Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto upper_panel = std::make_unique<ui::StackPanel>(Size(panel_width, upper_height), Colours::ItemDetails, Size(), StackPanel::Direction::Horizontal, SizeMode::Manual);
        upper_panel->set_margin(Size(5, 5));

        auto minimap_group = std::make_unique<GroupBox>(Size(370, 370), Colours::Triggers, Colours::DetailsBorder, L"Minimap");
        _minimap = minimap_group->add_child(std::make_unique<ui::Image>(Size(341, 341)));
        _map_tooltip = std::make_unique<Tooltip>(*minimap_group);
        upper_panel->add_child(std::move(minimap_group));

        right_panel->add_child(std::move(upper_panel));

        auto divider = std::make_unique<ui::Window>(Size(panel_width, 2), Colours::Divider);
        right_panel->add_child(std::move(divider));

        auto lower_panel = std::make_unique<ui::StackPanel>(Size(panel_width, window().size().height - upper_height - 2), Colours::ItemDetails, Size(), StackPanel::Direction::Horizontal, SizeMode::Manual);
        lower_panel->set_margin(Size(0, 2));

        auto lower_left = std::make_unique<StackPanel>(Size(190, 300), Colours::ItemDetails, Size(0, 2), StackPanel::Direction::Vertical, SizeMode::Manual);
        auto room_stats = std::make_unique<GroupBox>(Size(190, 150), Colours::ItemDetails, Colours::DetailsBorder, L"Room Details");
        _stats_box = room_stats->add_child(std::make_unique<Listbox>(Size(180, 150 - 21), Colours::LeftPanel));
        _stats_box->set_columns(
            {
                { Listbox::Column::Type::String, L"Name", 100 },
                { Listbox::Column::Type::String, L"Value", 70 },
            }
        );
        _stats_box->set_show_headers(false);

        _token_store += _stats_box->on_item_selected += [this](const auto& item)
        {
            if (item.value(L"Name") == L"Alternate")
            {
                on_room_selected(std::stoi(item.value(L"Value")));
            }
        };
        lower_left->add_child(std::move(room_stats));
        create_neighbours_list(*lower_left);

        lower_panel->add_child(std::move(lower_left));

        auto lower_right = std::make_unique<StackPanel>(Size(190, 300), Colours::ItemDetails, Size(0, 2), StackPanel::Direction::Vertical, SizeMode::Manual);
        create_items_list(*lower_right);
        create_triggers_list(*lower_right);
        lower_panel->add_child(std::move(lower_right));

        right_panel->add_child(std::move(lower_panel));
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
            if (_selected_trigger.has_value())
            {
                set_selected_trigger(_selected_trigger.value());
            }
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
}