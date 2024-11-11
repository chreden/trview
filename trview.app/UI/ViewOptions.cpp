#include "ViewOptions.h"
#include "../Windows/IViewer.h"

namespace trview
{
    IViewOptions::~IViewOptions()
    {
    }

    ViewOptions::ViewOptions()
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
        _toggles[IViewer::Options::notes] = true;
        _toggles[IViewer::Options::sound_sources] = false;
        _toggles[IViewer::Options::ng_plus] = false;
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
                add_toggle(IViewer::Options::depth_enabled);
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-1);
                if (ImGui::InputInt(IViewer::Options::depth.c_str(), &_scalars[IViewer::Options::depth], 1, 10))
                {
                    _scalars[IViewer::Options::depth] = std::max(0, _scalars[IViewer::Options::depth]);
                    on_scalar_changed(IViewer::Options::depth, _scalars[IViewer::Options::depth]);
                }
                ImGui::PopItemWidth();
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::wireframe);
                add_toggle(IViewer::Options::geometry);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::lighting);
                add_toggle(IViewer::Options::notes);
                ImGui::TableNextRow();
                add_toggle(IViewer::Options::sound_sources);
                ImGui::BeginDisabled(!_ng_plus_enabled); 
                add_toggle(IViewer::Options::ng_plus);
                ImGui::EndDisabled();
                ImGui::TableNextRow();
                if (!_use_alternate_groups)
                {
                    ImGui::BeginDisabled(!_flip_enabled);
                    add_toggle(IViewer::Options::flip);
                    ImGui::EndDisabled();
                }
                ImGui::EndTable();
            }

            if (_use_alternate_groups)
            {
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
