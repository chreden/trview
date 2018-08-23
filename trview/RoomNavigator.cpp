#include "stdafx.h"
#include "RoomNavigator.h"
#include "ITextureStorage.h"
#include "RoomInfo.h"

#include <trview.ui/GroupBox.h>
#include <trview.ui/Label.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/StackPanel.h>

namespace trview
{
    RoomNavigator::RoomNavigator(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto rooms_groups = std::make_unique<GroupBox>(Point(), Size(140, 130), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Rooms");
        auto highlight = std::make_unique<Checkbox>(Point(12, 20), Size(16, 16), L"Highlight");
        auto flip = std::make_unique<Checkbox>(Point(76, 20), Size(16, 16), L"Flip");

        highlight->on_state_changed += on_highlight;
        _highlight = highlight.get();

        flip->on_state_changed += on_flip;
        _flip = flip.get();

        auto room_box = std::make_unique<GroupBox>(Point(12, 40), Size(120, 80), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Room");
        auto room_controls = std::make_unique<StackPanel>(Point(12, 12), Size(96, 60), Colour(1.f, 0.5f, 0.5f, 0.5f), Size(),StackPanel::Direction::Vertical);
        auto room_number_labels = std::make_unique<StackPanel>(Point(), Size(96, 30), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(), StackPanel::Direction::Horizontal);
        auto room_number = std::make_unique<NumericUpDown>(Point(), Size(40, 20), Colour(1.0f, 0.4f, 0.4f, 0.4f), texture_storage.lookup("numeric_up"), texture_storage.lookup("numeric_down"), 0, 0);
        auto room_number_label = std::make_unique<Label>(Point(),Size(8, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L" / ", 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Centre, SizeMode::Auto);
        auto room_max_label = std::make_unique<Label>(Point(), Size(40, 20), Colour(1.0f, 0.4f, 0.4f, 0.4f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);

        room_number->on_value_changed += on_room_selected;
        _current = room_number.get();
        _max = room_max_label.get();

        room_number_labels->add_child(std::move(room_number));
        room_number_labels->add_child(std::move(room_number_label));
        room_number_labels->add_child(std::move(room_max_label));

        auto info_labels = std::make_unique<StackPanel>(Point(), Size(96, 36), Colour(1.f, 0.5f, 0.5f, 0.5f), Size(5, 5), StackPanel::Direction::Horizontal);
        auto x_label = std::make_unique<Label>(Point(), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"X:", 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Centre);
        auto z_label = std::make_unique<Label>(Point(), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Z:", 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Centre);

        _x = x_label.get();
        _z = z_label.get();

        info_labels->add_child(std::move(x_label));
        info_labels->add_child(std::move(z_label));

        room_controls->add_child(std::move(room_number_labels));
        room_controls->add_child(std::move(info_labels));

        room_box->add_child(std::move(room_controls));

        rooms_groups->add_child(std::move(highlight));
        rooms_groups->add_child(std::move(flip));
        rooms_groups->add_child(std::move(room_box));

        parent.add_child(std::move(rooms_groups));
    }

    void RoomNavigator::set_room_info(RoomInfo room_info)
    {
        _x->set_text(L"X:" + std::to_wstring(room_info.x / 1024));
        _z-> set_text(L"Z:" + std::to_wstring(room_info.z / 1024));
    }

    void RoomNavigator::set_max_rooms(uint32_t max_rooms)
    {
        uint32_t adjusted_size = max_rooms ? max_rooms - 1 : 0u;
        _current->set_maximum(adjusted_size);
        _max->set_text(std::to_wstring(adjusted_size));
    }

    void RoomNavigator::set_highlight(bool highlight)
    {
        _highlight->set_state(highlight);
    }

    void RoomNavigator::set_selected_room(uint32_t selected_room)
    {
        _current->set_value(selected_room);
    }

    void RoomNavigator::set_flip(bool flip)
    {
        _flip->set_state(flip);
    }

    void RoomNavigator::set_flip_enabled(bool enabled)
    {
        _flip->set_enabled(enabled);
    }
}
