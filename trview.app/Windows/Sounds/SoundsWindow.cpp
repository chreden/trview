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

    SoundsWindow::SoundsWindow()
    {
        setup_filters();
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

    void SoundsWindow::set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        _global_selected_sound_source = sound_source;
        if (_sync_sound_source)
        {
            _scroll_to_sound_source = true;
            set_local_selected_sound_source(sound_source);
        }
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
            _filters.render();
            ImGui::SameLine();
            bool sync_sound_source = _sync_sound_source;
            if (ImGui::Checkbox(Names::sync_sound_source.c_str(), &sync_sound_source))
            {
                set_sync_sound_source(sync_sound_source);
            }

            RowCounter counter{ "sound sources", _all_sound_sources.size() };
            if (ImGui::BeginTable(Names::sound_sources_list.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(200, -counter.height())))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(0));
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(1));
                ImGui::TableSetupColumn("Sample", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(2));
                ImGui::TableSetupColumn("Hide", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(3));
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort_weak(_all_sound_sources,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return l.id() < r.id(); },
                        [](auto&& l, auto&& r) { return l.sample() < r.sample(); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& sound_source : _all_sound_sources)
                {
                    auto sound_source_ptr = sound_source.lock();
                    if (!sound_source_ptr || !_filters.match(*sound_source_ptr))
                    {
                        continue;
                    }

                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto selected_sound_source = _selected_sound_source.lock();
                    bool selected = selected_sound_source && selected_sound_source == sound_source_ptr;

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_sound_source)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_sound_source = false;
                    }

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}##{0}", sound_source_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                         scroller.fix_scroll();
                         set_local_selected_sound_source(sound_source);
                         if (_sync_sound_source)
                         {
                             on_sound_source_selected(sound_source);
                         }
                         _scroll_to_sound_source = false;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(sound_source_ptr->id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(sound_source_ptr->sample()).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !sound_source_ptr->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", sound_source_ptr->number()).c_str(), &hidden))
                    {
                        sound_source_ptr->set_visible(!hidden);
                        on_scene_changed();
                    }
                    ImGui::PopStyleVar();
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
                ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav));
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
                        const auto sound = storage->get(i).lock();
                        if (!sound)
                        {
                            ImGui::BeginDisabled();
                        }
                        
                        if (ImGui::Button(std::format("Sample {}", i).c_str(), ImVec2(-1, 40)))
                        {
                            if (sound)
                            {
                                sound->play();
                            }
                        }

                        if (!sound)
                        {
                            ImGui::EndDisabled();
                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                            {
                                ImGui::SetTooltip("MAIN.SFX not found. Place MAIN.SFX in the same folder as the level.");
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
        if (ImGui::BeginChild("##soundboard"))
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
            ImGui::EndChild();
        }
    }

    void SoundsWindow::set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage)
    {
        _sound_storage = sound_storage;
    }

    void SoundsWindow::set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources)
    {
        _all_sound_sources = sound_sources;
        setup_filters();
        _force_sort = true;
        calculate_column_widths();
    }

    void SoundsWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("ID__", 1);
        _column_sizer.measure("Sample__", 2);
        _column_sizer.measure("Hide____", 3);

        for (const auto& sound_source : _all_sound_sources)
        {
            if (auto sound_source_ptr = sound_source.lock())
            {
                _column_sizer.measure(std::to_string(sound_source_ptr->number()), 0);
                _column_sizer.measure(std::to_string(sound_source_ptr->id()), 1);
                _column_sizer.measure(std::to_string(sound_source_ptr->sample()), 2);
            }
        }
    }

    void SoundsWindow::set_local_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        _selected_sound_source = sound_source;
    }

    void SoundsWindow::set_sync_sound_source(bool value)
    {
        if (_sync_sound_source != value)
        {
            _sync_sound_source = value;
            _scroll_to_sound_source = true;
            if (_sync_sound_source && _global_selected_sound_source.lock())
            {
                set_selected_sound_source(_global_selected_sound_source);
            }
        }
    }

    void SoundsWindow::setup_filters()
    {
        _filters.clear_all_getters();
        _filters.add_getter<float>("#", [](auto&& sound_source) { return static_cast<float>(sound_source.number()); });
        _filters.add_getter<float>("X", [](auto&& sound_source) { return sound_source.position().x * trlevel::Scale_X; });
        _filters.add_getter<float>("Y", [](auto&& sound_source) { return sound_source.position().y * trlevel::Scale_Y; });
        _filters.add_getter<float>("Z", [](auto&& sound_source) { return sound_source.position().z * trlevel::Scale_Z; });
        _filters.add_getter<float>("ID", [](auto&& sound_source) { return static_cast<float>(sound_source.id()); });
        _filters.add_getter<float>("Flags", [](auto&& sound_source) { return static_cast<float>(sound_source.flags()); });
        _filters.add_getter<float>("Chance", [](auto&& sound_source) { return static_cast<float>(sound_source.chance()); });
        _filters.add_getter<float>("Characteristics", [](auto&& sound_source) { return static_cast<float>(sound_source.characteristics()); });
        _filters.add_getter<float>("Sample", [](auto&& sound_source) { return static_cast<float>(sound_source.sample()); });
        _filters.add_getter<float>("Volume", [](auto&& sound_source) { return static_cast<float>(sound_source.volume()); });
        if (_level_version >= trlevel::LevelVersion::Tomb3)
        {
            _filters.add_getter<float>("Pitch", [](auto&& sound_source) { return static_cast<float>(sound_source.pitch()); });
            _filters.add_getter<float>("Range", [](auto&& sound_source) { return static_cast<float>(sound_source.range()); });
        }
    }
}
