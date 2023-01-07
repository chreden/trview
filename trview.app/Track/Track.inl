#pragma once

#include <trview.common/Algorithms.h>

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
                bool value = v.value;
                if (ImGui::Checkbox(to_string(v.type).c_str(), &value) && value != v.value)
                {
                    v.value = value;
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

    template <Type... Args>
    template <Type T>
    Event<bool>& Track<Args...>::on_toggle()
    {
        static_assert(equals_any(T, Args...), "Type is not being tracked");

        for (auto& subject : state)
        {
            if (subject.type == T)
            {
                return subject.on_toggle;
            }
        }

        throw std::exception();
    }
}
