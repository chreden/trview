#include "SoundsWindow.h"
#include "../RowCounter.h"
#include "../../trview_imgui.h"
#include "../../Elements/SoundSource/ISoundSource.h"
#include "../../Sound/ISoundStorage.h"
#include "../../Sound/ISound.h"
#include <trlevel/trtypes.h>
#include "../../Elements/ILevel.h"

#include <map>

namespace trview
{
    ISoundsWindow::~ISoundsWindow()
    {
    }

    void SoundsWindow::render()
    {
        if (!render_sounds_window())
        {
            on_window_closed();
            return;
        }
    }

    void SoundsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void SoundsWindow::set_number(int32_t number)
    {
        _id = std::format("Sounds {}", number);
    }

    bool SoundsWindow::render_sounds_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("Sound Sources"))
                {
                    render_sound_sources_list();
                    ImGui::SameLine();
                    render_sounds_source_details();
                    _force_sort = false;
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Sound Board"))
                {
                    render_sound_board();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void SoundsWindow::render_sound_sources_list()
    {
        calculate_column_widths();
        if (ImGui::BeginChild(Names::sound_source_list_panel.c_str(), ImVec2(200, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            // _filters.render();

            // ImGui::SameLine();
            // _track.render();

            // ImGui::SameLine();
            // bool sync_item = _sync_item;
            // if (ImGui::Checkbox(Names::sync_item.c_str(), &sync_item))
            // {
            //     set_sync_item(sync_item);
            // }

            RowCounter counter{ "sound sources", _all_sound_sources.size() };
            if (ImGui::BeginTable(Names::sound_sources_list.c_str(), 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(200, -counter.height())))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(0));
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(1));
                // ImGui::TableSetupColumn("Room", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(1));
                // ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(3));
                // ImGui::TableSetupColumn("Hide", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(4));
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort_weak(_all_sound_sources,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return l.id() < r.id(); },
                        // [](auto&& l, auto&& r) { return std::tuple(item_room(l), l.number()) < std::tuple(item_room(r), r.number()); },
                        // [](auto&& l, auto&& r) { return std::tuple(l.type_id(), l.number()) < std::tuple(r.type_id(), r.number()); },
                        // [](auto&& l, auto&& r) { return std::tuple(l.type(), l.number()) < std::tuple(r.type(), r.number()); },
                        // [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& sound_source : _all_sound_sources)
                {
                    auto sound_source_ptr = sound_source.lock();
                    if (!sound_source_ptr)// || (_track.enabled<Type::Room>() && item_ptr->room().lock() != _current_room.lock() || !_filters.match(*item_ptr)))
                    {
                        continue;
                    }

                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto selected_sound_source = _selected_sound_source.lock();
                    bool selected = selected_sound_source && selected_sound_source == sound_source_ptr;

                    ImGuiScroller scroller;
                    // if (selected && _scroll_to_sound_source)
                    // {
                    //     scroller.scroll_to_item();
                    //     _scroll_to_item = false;
                    // }

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}##{0}", sound_source_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                         scroller.fix_scroll();
                         set_local_selected_sound_source(sound_source);
                    //     if (_sync_item)
                    //     {
                    //         on_item_selected(item);
                    //     }
                    //     _scroll_to_item = false;
                    }
                    // ImGui::Text("Sound!");

                    // if (item_is_virtual && ImGui::IsItemHovered())
                    // {
                    //     ImGui::BeginTooltip();
                    //     ImGui::Text("Virtual item generated by trview");
                    //     ImGui::EndTooltip();
                    // }

                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(sound_source_ptr->id()).c_str());
                    // ImGui::TableNextColumn();
                    // ImGui::Text(std::to_string(item_ptr->type_id()).c_str());
                    // ImGui::TableNextColumn();
                    // ImGui::Text(item_ptr->type().c_str());
                    // ImGui::TableNextColumn();
                    // bool hidden = !item_ptr->visible();
                    // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    // if (ImGui::Checkbox(std::format("##hide-{}", item_ptr->number()).c_str(), &hidden))
                    // {
                    //     item_ptr->set_visible(!hidden);
                    //     on_scene_changed();
                    // }
                    // ImGui::PopStyleVar();
                }
                ImGui::EndTable();
                counter.render();
            }
        }
        ImGui::EndChild();
    }

    void SoundsWindow::render_sounds_source_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            constexpr auto add_stat = [&]<typename T>(const std::string & name, const T && value, Colour colour = Colour::White)
            {
                const auto string_value = get_string(value);
                ImGui::TableNextColumn();
                if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                {
                    // _clipboard->write(to_utf16(string_value));
                    // _tooltip_timer = 0.0f;
                }
                // if (_level_version == trlevel::LevelVersion::Tomb4 && ImGui::IsItemHovered() && _tips.find(name) != _tips.end())
                // {
                //     ImGui::BeginTooltip();
                //     ImGui::Text(_tips[name].c_str());
                //     ImGui::EndTooltip();
                // }
                ImGui::TableNextColumn();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colour.r, colour.g, colour.b, colour.a));
                ImGui::Text(string_value.c_str());
                ImGui::PopStyleColor();
            };

            auto selected_sound_source = _selected_sound_source.lock();
            if (selected_sound_source)
            {
                ImGui::Text("Sound Source");
                if (ImGui::BeginTable(Names::stats_listbox.c_str(), 2, 0, ImVec2(-1, 0)))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableNextRow();

                    add_stat("#", selected_sound_source->number());
                    add_stat("ID", selected_sound_source->id());
                    add_stat("Flags", selected_sound_source->flags());
                    const auto pos = selected_sound_source->position() * trlevel::Scale;
                    add_stat("Position", std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z));
                    add_stat("Chance", selected_sound_source->chance());
                    add_stat("Characteristics", selected_sound_source->characteristics());
                    add_stat("Sample", selected_sound_source->sample());
                    add_stat("Volume", selected_sound_source->volume());
                    if (_level_version >= trlevel::LevelVersion::Tomb3)
                    {
                        add_stat("Pitch", selected_sound_source->pitch());
                        add_stat("Range", selected_sound_source->range());
                    }
                    ImGui::EndTable();
                }

                if (auto storage = _sound_storage.lock())
                {
                    const auto sample_index = selected_sound_source->sample();
                    const auto num_samples = std::min(1, (selected_sound_source->characteristics() & 0x00FC) >> 2);
                    for (auto i = sample_index; i < sample_index + num_samples; ++i)
                    {
                        if (auto sound = storage->get(i).lock())
                        {
                            if (ImGui::Button(std::format("Sample {}", i).c_str(), ImVec2(-1, 40)))
                            {
                                sound->play();
                            }
                        }
                    }
                }
            }
        }
        ImGui::EndChild();
    }

    void SoundsWindow::render_sound_board()
    {
        if (const auto sound_storage = _sound_storage.lock())
        {
            ImVec2 size = ImGui::GetWindowSize();
            const int slots = static_cast<int>(size.x / 57);
            int count = 0;
            for (const auto [index, sound] : sound_storage->sounds() | std::ranges::to<std::map<int16_t, std::weak_ptr<ISound>>>())
            {
                if (const auto sound_ptr = sound.lock())
                {
                    if (ImGui::Button(std::format("{}", index).c_str(), ImVec2(50, 50)))
                    {
                        sound_ptr->play();
                    }
                    if (++count % slots != 0)
                    {
                        ImGui::SameLine(0.0f, 5.0f);
                    }
                }
            }
        }
    }

    void SoundsWindow::set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage)
    {
        _sound_storage = sound_storage;
    }

    void SoundsWindow::set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources)
    {
        _all_sound_sources = sound_sources;
        // _triggered_by.clear();
        // setup_filters();
        _force_sort = true;
        calculate_column_widths();
    }

    void SoundsWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("ID__", 1);
        // _column_sizer.measure("Type__", 3);
        // _column_sizer.measure("Hide____", 4);

        // for (const auto& item : _all_items)
        // {
        //     if (auto item_ptr = item.lock())
        //     {
        //         const bool item_is_virtual = is_virtual(*item_ptr);
        //         _column_sizer.measure(std::format("{0}{1}##{0}", item_ptr->number(), item_is_virtual ? "*" : ""), 0);
        //         _column_sizer.measure(std::to_string(item_room(item_ptr)), 1);
        //         _column_sizer.measure(std::to_string(item_ptr->type_id()), 2);
        //         _column_sizer.measure(item_ptr->type(), 3);
        //     }
        // }
    }

    void SoundsWindow::set_local_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        _selected_sound_source = sound_source;
    }
}
