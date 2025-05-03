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

    void SoundsWindow::set_level_platform(trlevel::Platform platform)
    {
        _level_platform = platform;
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
            _auto_hider.check_focus();

            _filters.render();
            ImGui::SameLine();
            bool sync_sound_source = _sync_sound_source;
            if (ImGui::Checkbox(Names::sync_sound_source.c_str(), &sync_sound_source))
            {
                set_sync_sound_source(sync_sound_source);
            }

            _auto_hider.render();

            auto filtered_sound_sources =
                _all_sound_sources |
                std::views::filter([&](auto&& sound)
                    {
                        const auto sound_ptr = sound.lock();
                        return !(!sound_ptr || !_filters.match(*sound_ptr));
                    }) |
                std::views::transform([](auto&& sound) { return sound.lock(); }) |
                std::ranges::to<std::vector>();

            if (_auto_hider.apply(_all_sound_sources, filtered_sound_sources, _filters))
            {
                on_scene_changed();
            }

            RowCounter counter{ "sound source", _all_sound_sources.size() };
            if (ImGui::BeginTable(Names::sound_sources_list.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(200, -counter.height())))
            {
                imgui_header_row(
                    {
                        { "#", _column_sizer.size(0) },
                        { "ID", _column_sizer.size(1) },
                        { "Sample", _column_sizer.size(2) },
                        { .name = "Hide", .width = _column_sizer.size(3), .set_checked = [&](bool v)
                            {
                                std::ranges::for_each(filtered_sound_sources, [=](auto&& sound) { sound->set_visible(!v); });
                                on_scene_changed();
                            }, .checked = std::ranges::all_of(filtered_sound_sources, [](auto&& sound) { return !sound->visible(); })
                        }
                    });

                imgui_sort_weak(_all_sound_sources,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(l.id(), l.number()) < std::tuple(r.id(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.sample(), l.number()) < std::tuple(r.sample(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& sound_source : filtered_sound_sources)
                {
                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto selected_sound_source = _selected_sound_source.lock();
                    bool selected = selected_sound_source && selected_sound_source == sound_source;

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_sound_source)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_sound_source = false;
                    }

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}##{0}", sound_source->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
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
                    ImGui::Text(std::to_string(sound_source->id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(sound_source->sample().value_or(-1)).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !sound_source->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", sound_source->number()).c_str(), &hidden))
                    {
                        sound_source->set_visible(!hidden);
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
                    const auto pos = selected_sound_source->position() * trlevel::Scale;
                    add_stat("Position", std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z));
                    if (selected_sound_source->sample().has_value())
                    {
                        add_stat("Chance", selected_sound_source->chance());
                        add_stat("Characteristics", selected_sound_source->characteristics());
                        add_stat("Flags", selected_sound_source->flags());
                        add_stat("Sample", selected_sound_source->sample().value());
                        add_stat("Volume", selected_sound_source->volume());
                        if (_level_version >= trlevel::LevelVersion::Tomb3)
                        {
                            add_stat("Pitch", selected_sound_source->pitch());
                            add_stat("Range", selected_sound_source->range());
                        }
                    }
                    ImGui::EndTable();
                }

                if (auto storage = _sound_storage.lock())
                {
                    const auto sample_index = selected_sound_source->sample();
                    if (sample_index.has_value())
                    {
                        const auto num_samples = std::min(1, (selected_sound_source->characteristics() & 0x00FC) >> 2);
                        for (auto i = sample_index.value(); i < sample_index.value() + num_samples; ++i)
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
                                    if (_level_platform == trlevel::Platform::PSX)
                                    {
                                        ImGui::SetTooltip("Sample playback not yet supported for PSX levels.");
                                    }
                                    else
                                    {
                                        ImGui::SetTooltip("MAIN.SFX not found. Place MAIN.SFX in the same folder as the level.");
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        ImGui::BeginDisabled();
                        ImGui::Button("Missing Sample", ImVec2(-1, 40));
                        ImGui::EndDisabled();
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        {
                            ImGui::SetTooltip(std::format("Sound Map entry {} does not reference a valid sample.", selected_sound_source->id()).c_str());
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
                const int slots = static_cast<int>(size.x / 67);
                int count = 0;
                for (const auto [index, sound] : sound_storage->sounds())
                {
                    if (const auto sound_ptr = sound.lock())
                    {
                        if (ImGui::Button(std::format("Map:{}\nDetails:{}\nSample:{}", index.sound_map, index.sound_details, index.sample_index).c_str(), ImVec2(60, 50)))
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
                _column_sizer.measure(std::to_string(sound_source_ptr->sample().value_or(-1)), 2);
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
        _filters.add_getter<float>("Sample", [](auto&& sound_source) { return static_cast<float>(sound_source.sample().value_or(-1)); });
        _filters.add_getter<float>("Volume", [](auto&& sound_source) { return static_cast<float>(sound_source.volume()); });
        if (_level_version >= trlevel::LevelVersion::Tomb3)
        {
            _filters.add_getter<float>("Pitch", [](auto&& sound_source) { return static_cast<float>(sound_source.pitch()); });
            _filters.add_getter<float>("Range", [](auto&& sound_source) { return static_cast<float>(sound_source.range()); });
        }
    }
}
