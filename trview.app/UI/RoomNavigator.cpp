#include "RoomNavigator.h"
#include <trview.app/Elements/RoomInfo.h>

#include <trlevel/trtypes.h>
#include <trview.ui/Label.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Listbox.h>
#include "../Resources/resource.h"

namespace trview
{
    const std::string RoomNavigator::Names::current_room{ "current_room" };
    const std::string RoomNavigator::Names::max_rooms{ "max_rooms" };
    const std::string RoomNavigator::Names::stats{ "stats" };

    RoomNavigator::RoomNavigator(ui::Control& parent, const ui::ILoader& ui_source)
    {
        using namespace ui;

        auto navigator = parent.add_child(ui_source.load_from_resource(IDR_UI_ROOM_NAVIGATOR));
        
        _current = navigator->find<NumericUpDown>(Names::current_room);
        _current->on_value_changed += on_room_selected;

        _max = navigator->find<Label>(Names::max_rooms);
        _listbox = navigator->find<Listbox>(Names::stats);
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
