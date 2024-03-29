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
            bool rotation_changed = false;
            if (ImGui::InputFloat("Yaw", &_rotation_yaw, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                rotation_changed = true;
            }
            if (ImGui::InputFloat("Pitch", &_rotation_pitch, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                rotation_changed = true;
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

            if (rotation_changed)
            {
                float rotation_yaw = _rotation_yaw;
                float rotation_pitch = _rotation_pitch;
                if (_display_degrees)
                {
                    rotation_yaw = DirectX::XMConvertToRadians(rotation_yaw);
                    rotation_pitch = DirectX::XMConvertToRadians(rotation_pitch);
                }
                on_rotation_changed(rotation_yaw, rotation_pitch);
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
        if (yaw < 0)
        {
            yaw += maths::Pi2;
        }
        _rotation_yaw = _display_degrees ? DirectX::XMConvertToDegrees(yaw) : yaw;
        _rotation_pitch = _display_degrees ? DirectX::XMConvertToDegrees(pitch) : pitch;
    }

    void CameraPosition::set_display_degrees(bool value)
    {
        _display_degrees = value;
    }
}
