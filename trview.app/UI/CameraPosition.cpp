#include "CameraPosition.h"
#include <trlevel/ILevel.h>
#include <external/imgui/imgui.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    void CameraPosition::render()
    {
        if (ImGui::Begin("Camera Position", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::InputFloat("Yaw", &_rotation_yaw))
            {
                if (_display_degrees)
                {
                    _rotation_yaw = DirectX::XMConvertToRadians(_rotation_yaw);
                }
                on_rotation_changed(_rotation_yaw, _rotation_pitch);
            }
            if (ImGui::InputFloat("Pitch", &_rotation_pitch))
            {
                if (_display_degrees)
                {
                    _rotation_pitch = DirectX::XMConvertToRadians(_rotation_pitch);
                }
                on_rotation_changed(_rotation_yaw, _rotation_pitch);
            }
            ImGui::Separator();
            if (ImGui::InputFloat("X", &_position.x))
            {
                on_position_changed(_position / trlevel::Scale_X);
            }
            if (ImGui::InputFloat("Y", &_position.y))
            {
                on_position_changed(_position / trlevel::Scale_X);
            }
            if (ImGui::InputFloat("Z", &_position.z))
            {
                on_position_changed(_position / trlevel::Scale_X);
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
