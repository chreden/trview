#include "RoomNavigator.h"
#include <trview.app/Graphics/ITextureStorage.h>
#include <trview.app/Elements/RoomInfo.h>

#include <trlevel/trtypes.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Label.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/Listbox.h>

namespace trview
{
    RoomNavigator::RoomNavigator(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto rooms_groups = std::make_unique<GroupBox>(Size(150, 90), Colour::Transparent, Colour::Grey, L"Room");

        auto room_controls = std::make_unique<StackPanel>(Point(4, 12), Size(140, 30), Colour::Transparent, Size(), StackPanel::Direction::Horizontal);
        _current = room_controls->add_child(std::make_unique<NumericUpDown>(Point(), Size(50, 20), Colour::Transparent, texture_storage.lookup("numeric_up"), texture_storage.lookup("numeric_down"), 0, 0));
        _current->on_value_changed += on_room_selected;

        room_controls->add_child(std::make_unique<Label>(Point(), Size(40, 20), Colour::Transparent, L"of", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
        _max = room_controls->add_child(std::make_unique<Label>(Point(), Size(50, 20), Colour::Transparent, L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
        rooms_groups->add_child(std::move(room_controls));

        auto listbox = std::make_unique<Listbox>(Point(4, 36), Size(140, 80), Colour::Transparent);
        listbox->set_show_scrollbar(false);
        listbox->set_show_headers(false);
        listbox->set_columns(
            {
                { Listbox::Column::Type::String, L"Name", 50 },
                { Listbox::Column::Type::String, L"Value", 90 }
            });
        _listbox = rooms_groups->add_child(std::move(listbox));
        parent.add_child(std::move(rooms_groups));
    }

    void RoomNavigator::set_room_info(RoomInfo room_info)
    {
        _listbox->set_items(
            {
                {{{ L"Name", L"X" }, { L"Value", std::to_wstring(static_cast<uint32_t>(room_info.x / trlevel::Scale_X))}}},
                {{{ L"Name", L"Z" }, { L"Value", std::to_wstring(static_cast<uint32_t>(room_info.z / trlevel::Scale_Z))}}}
            });
    }

    void RoomNavigator::set_max_rooms(uint32_t max_rooms)
    {
        uint32_t adjusted_size = max_rooms ? max_rooms - 1 : 0u;
        _current->set_maximum(adjusted_size);
        _max->set_text(std::to_wstring(adjusted_size));
    }

    void RoomNavigator::set_selected_room(uint32_t selected_room)
    {
        _current->set_value(selected_room);
    }
}
