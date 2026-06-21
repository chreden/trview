#include "RoomNavigator.h"

namespace trview
{
    void RoomNavigator::render()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos + ImVec2(4, 4), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Room Navigator", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const int32_t previous_selection = _selected_room;
            int32_t new_selection = _selected_room;
            if (ImGui::InputInt(std::format("of {}##roomnumber", _max_rooms).c_str(), &new_selection, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                _selected_room = std::clamp(new_selection, 0, _max_rooms);
                if (_selected_room != previous_selection)
                {
                    on_room_selected(_selected_room);
                }
            }
        }
        ImGui::End();
    }

    void RoomNavigator::set_max_rooms(uint32_t max_rooms)
    {
        uint32_t adjusted_size = max_rooms ? max_rooms - 1 : 0u;
        _max_rooms = adjusted_size;
    }

    void RoomNavigator::set_selected_room(uint32_t selected_room)
    {
        _selected_room = selected_room;
    }
}
