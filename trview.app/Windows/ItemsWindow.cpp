#include "ItemsWindow.h"
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include "../trview_imgui.h"

// TODO:
// Sorting on triggered by

using namespace trview::graphics;

namespace trview
{
    ItemsWindow::ItemsWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        _tips["OCB"] = "Changes entity behaviour";
        _tips["Clear Body"] = "If true, removed when Bodybag is triggered";
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    void ItemsWindow::update_items(const std::vector<Item>& items)
    {
        set_items(items);
        if (_track_room)
        {
            set_current_room(_current_room);
        }
    }

    void ItemsWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
    }

    void ItemsWindow::clear_selected_item()
    {
        _selected_item.reset();
    }

    void ItemsWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
    }

    void ItemsWindow::set_track_room(bool value)
    {
        _track_room = value;
    }

    void ItemsWindow::set_sync_item(bool value)
    {
        if (_sync_item != value)
        {
            _sync_item = value;
            _scroll_to_item = true;
            if (_sync_item && _global_selected_item.has_value())
            {
                set_selected_item(_global_selected_item.value());
            }
        }
    }

    void ItemsWindow::set_selected_item(const Item& item)
    {
        _global_selected_item = item;
        if (_sync_item)
        {
            _scroll_to_item = true;
            _selected_item = item;
        }
    }

    std::optional<Item> ItemsWindow::selected_item() const
    {
        return _selected_item;
    }

    void ItemsWindow::render_items_list()
    {
        if (ImGui::BeginChild(Names::item_list_panel.c_str(), ImVec2(270, 0), true))
        {
            bool track_room = _track_room;
            if (ImGui::Checkbox(Names::track_room.c_str(), &track_room))
            {
                set_track_room(track_room);
            }
            ImGui::SameLine();
            bool sync_item = _sync_item;
            if (ImGui::Checkbox(Names::sync_item.c_str(), &sync_item))
            {
                set_sync_item(sync_item);
            }

            if (ImGui::BeginTable(Names::items_list.c_str(), 5, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort(_all_items,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return l.room() < r.room(); },
                        [](auto&& l, auto&& r) { return l.type_id() < r.type_id(); },
                        [](auto&& l, auto&& r) { return l.type() < r.type(); },
                        [](auto&& l, auto&& r) { return l.visible() < r.visible(); }
                    });

                for (const auto& item : _all_items)
                {
                    if (_track_room && item.room() != _current_room)
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_item.has_value() && _selected_item.value().number() == item.number();
                    imgui_scroll_to_item(selected, _scroll_to_item);
                    if (ImGui::Selectable((std::to_string(item.number()) + std::string("##") + std::to_string(item.number())).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                    {
                        _selected_item = item;
                        if (_sync_item)
                        {
                            on_item_selected(item);
                        }
                    }
                    ImGui::SetItemAllowOverlap();
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item.room()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item.type_id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_utf8(item.type()).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !item.visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox((std::string("##hide-") + std::to_string(item.number())).c_str(), &hidden))
                    {
                        on_item_visibility(item, !hidden);
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    void ItemsWindow::render_item_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            ImGui::Text("Item Details");
            if (ImGui::BeginTable(Names::item_stats.c_str(), 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                if (_selected_item.has_value())
                {
                    const auto& item = _selected_item.value();

                    auto add_stat = [&](const std::string& name, const std::string& value)
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _clipboard->write(to_utf16(value));
                            _tooltip_timer = 0.0f;
                        }
                        if (ImGui::IsItemHovered() && _tips.find(name) != _tips.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text(_tips[name].c_str());
                            ImGui::EndTooltip();
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text(value.c_str());
                    };

                    auto position_text = [&item]()
                    {
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(0) <<
                            item.position().x * trlevel::Scale_X << ", " <<
                            item.position().y * trlevel::Scale_Y << ", " <<
                            item.position().z * trlevel::Scale_Z;
                        return stream.str();
                    };

                    add_stat("Type", to_utf8(item.type()));
                    add_stat("#", std::to_string(item.number()));
                    add_stat("Position", position_text());
                    add_stat("Type ID", std::to_string(item.type_id()));
                    add_stat("Room", std::to_string(item.room()));
                    add_stat("Clear Body", to_utf8(format_bool(item.clear_body_flag())));
                    add_stat("Invisible", to_utf8(format_bool(item.invisible_flag())));
                    add_stat("Flags", to_utf8(format_binary(item.activation_flags())));
                    add_stat("OCB", to_utf8(format_bool(item.ocb())));
                }

                ImGui::EndTable();
            }
            if (ImGui::Button(Names::add_to_route_button.c_str(), ImVec2(-1, 30)))
            {
                on_add_to_route(_selected_item.value());
            }
            ImGui::Text("Triggered By");
            if (ImGui::BeginTable(Names::triggers_list.c_str(), 3, ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (_selected_item.has_value())
                {
                    const auto& item = _selected_item.value();
                    for (auto& trigger : item.triggers())
                    {
                        auto trigger_ptr = trigger.lock();
                        if (!trigger_ptr)
                        {
                            continue;
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = _selected_trigger.lock() == trigger_ptr;
                        if (ImGui::Selectable(std::to_string(trigger_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                        {
                            _selected_trigger = trigger;
                            set_track_room(false);
                            on_trigger_selected(trigger);
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(trigger_ptr->room()).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(to_utf8(trigger_type_name(trigger_ptr->type())).c_str());
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    bool ItemsWindow::render_items_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_items_list();
            ImGui::SameLine();
            render_item_details();

            if (_tooltip_timer.has_value())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Copied");
                ImGui::EndTooltip();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void ItemsWindow::render()
    {
        if (!render_items_window())
        {
            on_window_closed();
            return;
        }
    }

    void ItemsWindow::update(float delta)
    {
        if (_tooltip_timer.has_value())
        {
            _tooltip_timer = _tooltip_timer.value() + delta;
            if (_tooltip_timer.value() > 0.6f)
            {
                _tooltip_timer.reset();
            }
        }
    }

    void ItemsWindow::set_number(int32_t number)
    {
        _id = "Items " + std::to_string(number);
    }
}
