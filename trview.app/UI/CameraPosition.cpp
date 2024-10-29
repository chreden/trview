#include "CameraPosition.h"
#include <trlevel/ILevel.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    void CameraPosition::render()
    {
        if (!_visible)
        {
            return;
        }

        check_reposition();
        const bool was_visible = _visible;
        if (ImGui::Begin("Camera Position", &_visible, ImGuiWindowFlags_AlwaysAutoResize))
        {
            capture_position();

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

        if (!_visible && was_visible)
        {
            on_hidden();
        }
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

    void CameraPosition::set_visible(bool value)
    {
        _visible = value;
    }

    void CameraPosition::reposition()
    {
        _reposition = true;
    }

    void CameraPosition::check_reposition()
    {
        if (!_reposition || !_in_window_offset.has_value())
        {
            return;
        }

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos +
            ImVec2{ 0, ImGui::GetMainViewport()->Size.y } +
            _in_window_offset.value());
        _reposition = false;
        _in_window_offset.reset();
    }

    void CameraPosition::capture_position()
    {
        const auto window = ImGui::GetCurrentWindow();
        const auto viewport = ImGui::GetMainViewport();
        if (window->Viewport != viewport)
        {
            _in_window_offset.reset();
        }
        else
        {
            const auto pos = window->Pos - viewport->Pos;
            _in_window_offset = ImVec2{ pos.x, pos.y - viewport->Size.y };
        }
    }

    void CameraPosition::reset()
    {
        _in_window_offset = { {  4, -148 } };
        _reposition = true;
    }
}
