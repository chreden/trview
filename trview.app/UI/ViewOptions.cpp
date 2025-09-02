#include "ViewOptions.h"
#include "../Windows/IViewer.h"
#include "../Windows/IRoomsWindowManager.h"

namespace trview
{
    namespace
    {
        void show_room_filter(
            std::weak_ptr<IRoomsWindowManager> rooms_window_manager,
            const std::vector<Filters<IRoom>::Filter>& filters)
        {
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::BeginMenu("Filter"))
                {
                    if (auto rooms_windows = rooms_window_manager.lock())
                    {
                        if (ImGui::MenuItem("New Window"))
                        {
                            if (const auto new_window = rooms_windows->create_window().lock())
                            {
                                new_window->set_filters(filters);
                            }
                        }

                        for (const auto& window : rooms_windows->windows())
                        {
                            if (auto actual_window = window.lock())
                            {
                                if (ImGui::MenuItem(actual_window->name().c_str()))
                                {
                                    actual_window->set_filters(filters);
                                }
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        }
    }

    IViewOptions::~IViewOptions()
    {
    }

    ViewOptions::ViewOptions(const std::weak_ptr<IRoomsWindowManager>& rooms_window_manager)
        : _rooms_window_manager(rooms_window_manager)
    {
        _toggles[IViewer::Options::highlight] = false;
        _toggles[IViewer::Options::triggers] = true;
        _toggles[IViewer::Options::geometry] = false;
        _toggles[IViewer::Options::water] = true;
        _toggles[IViewer::Options::depth_enabled] = false;
        _toggles[IViewer::Options::wireframe] = false;
        _toggles[IViewer::Options::show_bounding_boxes] = false;
        _toggles[IViewer::Options::lights] = false;
        _toggles[IViewer::Options::flip] = false;
        _toggles[IViewer::Options::items] = true;
        _toggles[IViewer::Options::rooms] = true;
        _toggles[IViewer::Options::camera_sinks] = false;
        _toggles[IViewer::Options::lighting] = true;
        _toggles[IViewer::Options::animation] = true;
        _toggles[IViewer::Options::notes] = true;
        _toggles[IViewer::Options::sound_sources] = false;
        _toggles[IViewer::Options::ng_plus] = false;
        _toggles[IViewer::Options::horizontal_portals] = false;
        _toggles[IViewer::Options::vertical_portals] = false;
    }

    void ViewOptions::render()
    {
        auto add_toggle = [&](const std::string& name)
        {
            ImGui::TableNextColumn();
            if (ImGui::Checkbox(name.c_str(), &_toggles[name]))
            {
                on_toggle_changed(name, _toggles[name]);
            }
        };

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos + ImVec2(4, 161), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("View Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTable(Names::flags.c_str(), 2))
            {
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::items);
                add_toggle(IViewer::Options::triggers);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::lights);
                add_toggle(IViewer::Options::water);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::show_bounding_boxes);
                add_toggle(IViewer::Options::highlight);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::rooms);
                add_toggle(IViewer::Options::camera_sinks);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::wireframe);
                add_toggle(IViewer::Options::geometry);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::lighting);
                add_toggle(IViewer::Options::animation);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::notes);
                add_toggle(IViewer::Options::sound_sources);
                ImGui::TableNextRow();
                ImGui::BeginDisabled(!_ng_plus_enabled); 
                add_toggle(IViewer::Options::ng_plus);
                ImGui::EndDisabled();

                ImGui::TableNextRow();
                add_toggle(IViewer::Options::depth_enabled);
                ImGui::TableNextColumn();
                ImGui::BeginDisabled(!_toggles[IViewer::Options::depth_enabled]);
                ImGui::PushItemWidth(-1);
                if (ImGui::InputInt(IViewer::Options::depth.c_str(), &_scalars[IViewer::Options::depth], 1, 10))
                {
                    _scalars[IViewer::Options::depth] = std::max(0, _scalars[IViewer::Options::depth]);
                    on_scalar_changed(IViewer::Options::depth, _scalars[IViewer::Options::depth]);
                }
                ImGui::PopItemWidth();
                ImGui::EndDisabled();

                ImGui::TableNextRow();
                add_toggle(IViewer::Options::portals_enabled);
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-1);

                const bool horizontal_portals = _toggles[IViewer::Options::horizontal_portals];
                const bool vertical_portals = _toggles[IViewer::Options::vertical_portals];
                const std::string current_portal_option =
                    (horizontal_portals && vertical_portals) ? "All" :
                    horizontal_portals ? "Horizontal" :
                    vertical_portals ? "Vertical" :
                    "None";

                ImGui::BeginDisabled(!_toggles[IViewer::Options::portals_enabled]);
                if (ImGui::BeginCombo("##Portals", current_portal_option.c_str()))
                {
                    if (ImGui::Selectable("Horizontal"))
                    {
                        _toggles[IViewer::Options::horizontal_portals] = true;
                        _toggles[IViewer::Options::vertical_portals] = false;
                        on_toggle_changed(IViewer::Options::horizontal_portals, true);
                        on_toggle_changed(IViewer::Options::vertical_portals, false);
                    }
                    if (ImGui::Selectable("Vertical"))
                    {
                        _toggles[IViewer::Options::horizontal_portals] = false;
                        _toggles[IViewer::Options::vertical_portals] = true;
                        on_toggle_changed(IViewer::Options::horizontal_portals, false);
                        on_toggle_changed(IViewer::Options::vertical_portals, true);
                    }
                    if (ImGui::Selectable("All"))
                    {
                        _toggles[IViewer::Options::horizontal_portals] = true;
                        _toggles[IViewer::Options::vertical_portals] = true;
                        on_toggle_changed(IViewer::Options::horizontal_portals, true);
                        on_toggle_changed(IViewer::Options::vertical_portals, true);
                    }
                    ImGui::EndCombo();
                }
                ImGui::EndDisabled();
                ImGui::TableNextRow();
                if (!_use_alternate_groups)
                {
                    ImGui::BeginDisabled(!_flip_enabled);
                    add_toggle(IViewer::Options::flip);
                    ImGui::EndDisabled();

                    if (_flip_enabled)
                    {
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text("Click to toggle flip map, right click for filter options.");
                            ImGui::EndTooltip();
                        }
                        show_room_filter(_rooms_window_manager, { {.key = "Alternate", .compare = CompareOp::Exists, .op = Op::And } });
                    }
                }
                ImGui::EndTable();
            }

            if (_use_alternate_groups)
            {
                ImGui::Text(std::format("{}Alternate Groups", _alternate_group_values.empty() ? "No " : "").c_str());
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Alternate groups in the level. Click to toggle, right click for filter options.");
                    ImGui::EndTooltip();
                }

                for (auto& group : _alternate_group_values)
                {
                    std::string id = std::to_string(group.first) + "##" + std::to_string(group.first) + "_flip";
                    bool enabled = group.second;
                    if (enabled)
                    {
                        ImGui::PushID(id.c_str());
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.06f, 0.53f, 0.98f, 1.00f));
                        ImGui::PopID();
                    }
                    if (ImGui::Button(id.c_str()))
                    {
                        group.second = !group.second;
                        on_alternate_group(group.first, group.second);
                    }
                    if (enabled)
                    {
                        ImGui::PopStyleColor();
                    }
                    show_room_filter(_rooms_window_manager, { {.key = "Alternate Group", .compare = CompareOp::Equal, .value = std::to_string(group.first), .op = Op::And } });
                    ImGui::SameLine();
                }
            }
        }
        ImGui::End();
    }

    void ViewOptions::set_alternate_group(uint32_t value, bool enabled)
    {
        _alternate_group_values[value] = enabled;
    }

    void ViewOptions::set_alternate_groups(const std::set<uint32_t>& groups)
    {
        _alternate_group_values.clear();
        for (const auto& group : groups)
        {
            _alternate_group_values[group] = false;
        }
    }

    void ViewOptions::set_use_alternate_groups(bool value)
    {
        _use_alternate_groups = value;
    }

    void ViewOptions::set_scalar(const std::string& name, int32_t value)
    {
        _scalars[name] = value;
    }

    void ViewOptions::set_toggle(const std::string& name, bool value) 
    {
        _toggles[name] = value;
    }

    bool ViewOptions::toggle(const std::string& name) const 
    {
        auto toggle = _toggles.find(name);
        if (toggle == _toggles.end())
        {
            return false;
        }
        return toggle->second;
    }

    void ViewOptions::set_flip_enabled(bool enabled)
    {
        _flip_enabled = enabled;
    }

    void ViewOptions::set_ng_plus_enabled(bool enabled)
    {
        _ng_plus_enabled = enabled;
    }
}
