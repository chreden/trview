#include "stdafx.h"
#include "RoomNavigator.h"
#include "ITextureStorage.h"

#include <sstream>

#include <trview.ui/GroupBox.h>
#include <trview.ui/Label.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Button.h>
#include <trview.ui/StackPanel.h>

namespace trview
{
    RoomNavigator::RoomNavigator(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto rooms_groups = std::make_unique<GroupBox>(Point(), Size(140, 130), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Rooms");
        auto highlight = std::make_unique<Button>(Point(12, 20), Size(16, 16), texture_storage.untextured(), texture_storage.untextured());
        auto highlight_label = std::make_unique<Label>(Point(32, 20), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Highlight", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        highlight->on_click += [&]() { toggle_highlight(); };
        _highlight_button = highlight.get();

        auto room_box = std::make_unique<GroupBox>(Point(12, 40), Size(120, 80), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Room");
        auto room_controls = std::make_unique<StackPanel>(Point(12, 12), Size(96, 60), Colour(1.f, 0.5f, 0.5f, 0.5f), Size(0, 0),StackPanel::Direction::Vertical);
        auto room_number_labels = std::make_unique<StackPanel>(Point(), Size(96, 30), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(5, 5), StackPanel::Direction::Horizontal);
        auto room_number = std::make_unique<NumericUpDown>(Point(90, 16), Size(40, 20), Colour(1.0f, 0.4f, 0.4f, 0.4f), texture_storage.untextured(), texture_storage.untextured(), 0, 0);
        auto room_number_label = std::make_unique<Label>(Point(),Size(8, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"/", 16.f, TextAlignment::Left, ParagraphAlignment::Centre);
        auto room_max_label = std::make_unique<Label>(Point(), Size(40, 20), Colour(1.0f, 0.4f, 0.4f, 0.4f), L"0", 10.f, TextAlignment::Centre, ParagraphAlignment::Centre);

        room_number->on_value_changed += [&](int32_t value)
        {
            on_room_selected.raise(value);
        };
        _current_room = room_number.get();
        _max_rooms = room_max_label.get();

        room_number_labels->add_child(std::move(room_number));
        room_number_labels->add_child(std::move(room_number_label));
        room_number_labels->add_child(std::move(room_max_label));

        auto room_info_labels = std::make_unique<StackPanel>(Point(12, 12),Size(96, 36),Colour(1.f, 0.5f, 0.5f, 0.5f),Size(5, 5),StackPanel::Direction::Horizontal);
        auto room_x_label = std::make_unique<Label>(Point(), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"X:", 10.f, TextAlignment::Left, ParagraphAlignment::Centre);
        auto room_z_label = std::make_unique<Label>(Point(), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Z:", 10.f, TextAlignment::Left,ParagraphAlignment::Centre);

        _room_x = room_x_label.get();
        _room_z = room_z_label.get();

        room_info_labels->add_child(std::move(room_x_label));
        room_info_labels->add_child(std::move(room_z_label));

        room_controls->add_child(std::move(room_number_labels));
        room_controls->add_child(std::move(room_info_labels));

        room_box->add_child(std::move(room_controls));

        rooms_groups->add_child(std::move(highlight));
        rooms_groups->add_child(std::move(highlight_label));
        rooms_groups->add_child(std::move(room_box));

        parent.add_child(std::move(rooms_groups));
    }

    void RoomNavigator::set_room_info(RoomInfo room_info)
    {
        std::wstringstream stream;
        stream << L"X: " << room_info.x / 1024.0f;
        _room_x->set_text(stream.str());
        stream.str(L"");
        stream << L"Z: " << room_info.z / 1024.0f;
        _room_z-> set_text(stream.str());
    }

    void RoomNavigator::set_max_rooms(uint32_t max_rooms)
    {
        _current_room->set_maximum(max_rooms);
        std::wstringstream stream;
        stream << max_rooms;
        _max_rooms->set_text(stream.str());
    }

    void RoomNavigator::set_highlight(bool highlighted)
    {
        _highlight = highlighted;
    }

    void RoomNavigator::set_selected_room(uint32_t selected_room)
    {
        _current_room->set_value(selected_room);
    }

    void RoomNavigator::toggle_highlight()
    {
        _highlight = !_highlight;
        on_highlight.raise(_highlight);
    }
}
