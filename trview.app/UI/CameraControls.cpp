#include "CameraControls.h"

namespace trview
{
    ICameraControls::~ICameraControls()
    {
    }

    // Set the current camera mode and raise the on_mode_selected event.
    // mode: The new camera mode.
    void CameraControls::change_mode(ICamera::Mode mode)
    {
        _mode = mode;
        on_mode_selected(_mode);
    }

    void CameraControls::change_projection(ProjectionMode mode)
    {
        _projection_mode = mode;
        on_projection_mode_selected(_projection_mode);
    }

    void CameraControls::render()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos + ImVec2(4, 59), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Camera Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const std::array<std::string, 3> mode_items{ "Orbit", "Free", "Axis" };
            if (ImGui::BeginCombo(Names::mode.c_str(), mode_items[static_cast<uint32_t>(_mode)].c_str()))
            {
                for (std::size_t n = 0; n < mode_items.size(); ++n)
                {
                    bool is_selected = _mode == static_cast<ICamera::Mode>(n);
                    if (ImGui::Selectable(mode_items[n].c_str(), is_selected))
                    {
                        change_mode(static_cast<ICamera::Mode>(n));
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            const std::array<std::string, 2> projection_items{ "Perspective", "Orthographic" };
            if (ImGui::BeginCombo(Names::projection_mode.c_str(), projection_items[static_cast<uint32_t>(_projection_mode)].c_str()))
            {
                for (std::size_t n = 0; n < projection_items.size(); ++n)
                {
                    bool is_selected = _projection_mode == static_cast<ProjectionMode>(n);
                    if (ImGui::Selectable(projection_items[n].c_str(), is_selected))
                    {
                        change_projection(static_cast<ProjectionMode>(n));
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button(Names::reset.c_str()))
            {
                on_reset();
            }
        }
        ImGui::End();
    }

    // Set the current camera mode. This will not raise the on_mode_selected event.
    // mode: The camera mode to change to.
    void CameraControls::set_mode(ICamera::Mode mode)
    {
        _mode = mode;
    }

    void CameraControls::set_projection_mode(ProjectionMode mode)
    {
        _projection_mode = mode;
    }
}
