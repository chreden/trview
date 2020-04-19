#include "RoomsWindow.h"
#include <trview.ui/StackPanel.h>
#include <trview.app/Elements/Room.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>
#include <numeric>

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

        ui::Listbox::Item create_listbox_item(const Room* const room, const std::vector<Item>& items, const std::vector<Trigger*>& triggers)
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
    }

    RoomsWindow::RoomsWindow(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Window& parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.rooms", L"Rooms", Size(470, 400))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_left_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<ui::StackPanel>(Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto controls = std::make_unique<StackPanel>(Size(200, 20), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Horizontal, SizeMode::Manual);
        controls->set_margin(Size(2, 2));
        auto track_room = std::make_unique<Checkbox>(Colours::LeftPanel, L"Track Room");
        _token_store += track_room->on_state_changed += [this](bool value)
        {
            // set_track_room(value);
        };

        _track_room_checkbox = controls->add_child(std::move(track_room));

        _controls = left_panel->add_child(std::move(controls));

        auto rooms_list = std::make_unique<Listbox>(Size(200, window().size().height - _controls->size().height), Colours::LeftPanel);
        rooms_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Items", 50 },
                { Listbox::Column::Type::Number, L"Triggers", 50 }
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

        _rooms_list = left_panel->add_child(std::move(rooms_list));

        // Fix items list size now that it has been added to the panel.
        _rooms_list->set_size(Size(200, left_panel->size().height - _rooms_list->position().y));

        return left_panel;
    }

    void RoomsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    void RoomsWindow::set_rooms(const std::vector<Room*>& rooms)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(rooms.begin(), rooms.end(), std::back_inserter(list_items), [&](const auto& room) { return create_listbox_item(room, _all_items, _all_triggers); });
        _rooms_list->set_items(list_items);
        _all_rooms = rooms;
    }

    void RoomsWindow::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
    }

    void RoomsWindow::load_room_details(const Room& room)
    {

    }

    std::unique_ptr<ui::Control> RoomsWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        return right_panel;
    }
}