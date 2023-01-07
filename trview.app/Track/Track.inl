#pragma once

namespace trview
{
    template <Type... Args>
    void Track<Args...>::render()
    {
        bool filter_enabled = true;
        if (ImGui::Checkbox("##TrackEnable", &filter_enabled))
        {
            _enabled = filter_enabled;
        }
        ImGui::SameLine();

        if (ImGui::Button("Track##track"))
        {
            toggle_visible();
        }

        if (_show && ImGui::BeginPopup("Track"))
        {
            for (Subject& v : state)
            {
                if (ImGui::Checkbox(to_string(v.type).c_str(), &v.value))
                {
                    v.on_toggle(v.value);
                }
            }
            ImGui::EndPopup();
        }
        else
        {
            _show = false;
        }
    }

    template <Type... Args>
    void Track<Args...>::toggle_visible()
    {
        if (!_show)
        {
            ImGui::OpenPopup("Track");
        }
        _show = !_show;
    }

    template <Type... Args>
    bool Track<Args...>::enabled() const
    {
        return _enabled;
    }

    template <Type... Args>
    bool Track<Args...>::enabled(Type type) const
    {
        for (const auto& subject : state)
        {
            if (subject.type == type)
            {
                return subject.value;
            }
        }
        return false;
    }
}
