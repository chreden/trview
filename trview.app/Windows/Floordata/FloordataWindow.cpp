#include "FloordataWindow.h"
#include "../../Elements/Floordata.h"
#include "../../Elements/ISector.h"
#include "../../Elements/IRoom.h"

namespace trview
{
    IFloordataWindow::~IFloordataWindow()
    {
    }

    bool FloordataWindow::render_floordata_window()
    {
        bool stay_open = true;

        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            ImGui::Checkbox(Names::track_room.c_str(), &_track_room);
            ImGui::SameLine();
            ImGui::Checkbox(Names::simple_mode.c_str(), &_simple_mode);
            if (_simple_mode)
            {
                render_simple();
            }
            else
            {
                render_advanced();
            }
        }
        ImGui::End();
        return stay_open;
    }

    void FloordataWindow::render_simple()
    {
        if (ImGui::BeginTable("##floordata", 4, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Room");
            ImGui::TableSetupColumn("Sector");
            ImGui::TableSetupScrollFreeze(1, 1);
            ImGui::TableHeadersRow();

            std::weak_ptr<ISector> previous_sector;
            for (uint32_t i = 0; i < _floordata.size(); ++i)
            {
                const uint32_t index = i;
                const auto found = _simple_map.find(index);
                if (_track_room && found != _simple_map.end())
                {
                    auto found_ptr = found->second.lock();
                    if (found_ptr && found_ptr->room() != _current_room)
                    {
                        continue;
                    }
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                bool is_selected = index == _selected_floordata;
                if (ImGui::Selectable(std::to_string(index).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)) || ImGui::IsItemHovered())
                {
                    _selected_floordata = index;
                    if (found != _simple_map.end())
                    {
                        on_sector_highlight(found->second);
                    }
                }

                ImGui::TableNextColumn();
                ImGui::Text("%04X", _floordata[index]);

                ImGui::TableNextColumn();
                if (found != _simple_map.end() && index != 0)
                {
                    if (auto used_ptr = found->second.lock())
                    {
                        if (used_ptr != previous_sector.lock())
                        {
                            ImGui::Text("%d", used_ptr->room());
                        }
                    }
                }
                else
                {
                    ImGui::Text("-");
                }
                ImGui::TableNextColumn();
                if (found != _simple_map.end() && index != 0)
                {
                    if (auto used_ptr = found->second.lock())
                    {
                        if (used_ptr != previous_sector.lock())
                        {
                            ImGui::Text("%d, %d", used_ptr->x(), used_ptr->z());
                        }
                    }
                }
                else
                {
                    ImGui::Text("-");
                }

                if (found != _simple_map.end())
                {
                    previous_sector = found->second;
                }
                else
                {
                    previous_sector.reset();
                }
            }

            ImGui::EndTable();
        }
    }

    void FloordataWindow::render_advanced()
    {
        if (ImGui::BeginTable("##floordata", 5, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Meaning");
            ImGui::TableSetupColumn("Room");
            ImGui::TableSetupColumn("Sector");
            ImGui::TableSetupScrollFreeze(1, 1);
            ImGui::TableHeadersRow();

            std::weak_ptr<ISector> previous_sector;
            for (uint32_t i = 0; i < _floordata.size(); ++i)
            {
                const uint32_t index = i;
                const auto floordata = parse_floordata(_floordata, index, FloordataMeanings::Generate, _items);
                i += floordata.size() - 1;
                const auto found = _floordata_map.find(index);

                if (_track_room &&
                    found != _floordata_map.end() &&
                    !std::any_of(found->second.begin(), found->second.end(),
                        [this](auto&& u) 
                        {
                            auto u_ptr = u.lock();
                            return u_ptr && u_ptr->room() == _current_room; 
                        }))
                {
                    if (found != _floordata_map.end() && !found->second.empty())
                    {
                        previous_sector = found->second[0];
                    }
                    continue;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bool is_selected = index == _selected_floordata;
                if (ImGui::Selectable(std::to_string(index).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)) || ImGui::IsItemHovered())
                {
                    _selected_floordata = index;
                    if (found != _floordata_map.end())
                    {
                        if (!found->second.empty())
                        {
                            on_sector_highlight(found->second[0]);
                        }
                    }
                }
                ImGui::TableNextColumn();

                for (const auto& command : floordata.commands)
                {
                    for (const auto& data : command.data)
                    {
                        ImGui::Text("%04X", data);
                    }
                }
                ImGui::TableNextColumn();
                for (const auto& command : floordata.commands)
                {
                    for (const auto& meaning : command.meanings)
                    {
                        ImGui::Text(meaning.c_str());
                    }
                }
                ImGui::TableNextColumn();
                if (found != _floordata_map.end() && index != 0)
                {
                    for (const auto& used : found->second)
                    {
                        if (auto used_ptr = used.lock())
                        {
                            if (used_ptr != previous_sector.lock())
                            {
                                ImGui::Text("%d", used_ptr->room());
                            }
                        }
                    }
                }
                else
                {
                    ImGui::Text("-");
                }
                ImGui::TableNextColumn();
                if (found != _floordata_map.end() && index != 0)
                {
                    for (const auto& used : found->second)
                    {
                        if (auto used_ptr = used.lock())
                        {
                            if (used_ptr != previous_sector.lock())
                            {
                                ImGui::Text("%d, %d", used_ptr->x(), used_ptr->z());
                            }
                        }
                    }
                }
                else
                {
                    ImGui::Text("-");
                }

                if (found != _floordata_map.end() && !found->second.empty())
                {
                    previous_sector = found->second[0];
                }
            }

            ImGui::EndTable();
        }
    }

    void FloordataWindow::render()
    {
        if (!render_floordata_window())
        {
            on_window_closed();
        }
    }

    void FloordataWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
    }

    void FloordataWindow::set_floordata(const std::vector<uint16_t>& data)
    {
        _floordata = data;

        // Generate simple map:
        _simple_map.clear();
        for (const auto& room : _rooms)
        {
            auto room_ptr = room.lock();
            if (room_ptr)
            {
                for (const auto& sector : room_ptr->sectors())
                {
                    if (sector->floordata_index() != 0)
                    {
                        const auto floordata = parse_floordata(_floordata, sector->floordata_index(), FloordataMeanings::None, _items);
                        for (uint32_t i = sector->floordata_index(); i < sector->floordata_index() + floordata.size(); ++i)
                        {
                            _simple_map[i] = sector;
                        }
                    }
                }
            }
        }
    }

    void FloordataWindow::update(float)
    {

    }

    void FloordataWindow::set_items(const std::vector<Item>& items)
    {
        _items = items;
    }

    void FloordataWindow::set_number(int32_t number)
    {
        _id = std::format("Floordata {}", number);
    }

    void FloordataWindow::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _rooms = rooms;

        // Generate advanced map:
        _floordata_map.clear();
        for (const auto& room : _rooms)
        {
            auto room_ptr = room.lock();
            if (room_ptr)
            {
                for (const auto& sector : room_ptr->sectors())
                {
                    if (sector->floordata_index() != 0)
                    {
                        _floordata_map[sector->floordata_index()].push_back(sector);
                    }
                }
            }
        }
    }
}
