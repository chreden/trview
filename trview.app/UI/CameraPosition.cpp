#include "CameraPosition.h"
#include <trlevel/ILevel.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    void CameraPosition::render()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos + ImVec2(4, ImGui::GetMainViewport()->Size.y - 148), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Camera Position", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            bool yaw_changed = false;
            bool pitch_changed = false;

            if (ImGui::InputFloat("Yaw", &_rotation_yaw, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                yaw_changed = true;
            }
            if (ImGui::InputFloat("Pitch", &_rotation_pitch, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pitch_changed = true;
            }
            ImGui::Separator();
            if (ImGui::InputFloat("X", &_position.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                on_position_changed(_position / trlevel::Scale_X);
            }
            if (ImGui::InputFloat("Y", &_position.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                on_position_changed(_position / trlevel::Scale_X);
            }
            if (ImGui::InputFloat("Z", &_position.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                on_position_changed(_position / trlevel::Scale_X);
            }

            if (yaw_changed || pitch_changed)
            {
                if (_display_degrees)
                {
                    if (yaw_changed)
                    {
                        _rotation_yaw = DirectX::XMConvertToRadians(_rotation_yaw);
                    }
                    if (pitch_changed)
                    {
                        _rotation_pitch = DirectX::XMConvertToRadians(_rotation_pitch);
                    }
                }
                on_rotation_changed(_rotation_yaw, _rotation_pitch);
            }
        }
        ImGui::End();
    }

    void CameraPosition::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _position = position * trlevel::Scale_X;
    }

    void CameraPosition::set_rotation(float yaw, float pitch)
    {
        _rotation_yaw = _display_degrees ? DirectX::XMConvertToDegrees(yaw) : yaw;
        _rotation_pitch = _display_degrees ? DirectX::XMConvertToDegrees(pitch) : pitch;
    }

    void CameraPosition::set_display_degrees(bool value)
    {
        _display_degrees = value;
    }
}
