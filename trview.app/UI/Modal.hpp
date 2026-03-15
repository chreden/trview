#pragma once

namespace trview
{
    template <typename State>
    void Modal<State>::show(const State& state)
    {
        _state = state;
        _open = true;
    }

    template <typename State>
    void Modal<State>::render(const std::function<bool (State&)>& callback)
    {
        if (!_open)
        {
            return;
        }

        if (_open.value())
        {
            ImGui::OpenPopup("Enter Filter Name");
            _open = false;
            _is_open = true;
        }

        if (ImGui::BeginPopupModal("Enter Filter Name", &_is_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (!callback(_state))
            {
                ImGui::CloseCurrentPopup();
                _open.reset();
            }
            ImGui::EndPopup();
        }

        if (!_open)
        {
            _open.reset();
        }
    }
}
