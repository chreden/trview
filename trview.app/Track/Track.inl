#pragma once

#include <ranges>

#include <trview.common/Algorithms.h>

namespace trview
{
    template <Type... Args>
    void Track<Args...>::render()
    {
        if (ImGui::Checkbox(Names::Enable.c_str(), &_enabled))
        {
            std::ranges::for_each(state, [=](auto& s) { s.on_toggle(s.value && _enabled); });
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Toggle tracking");
            ImGui::EndTooltip();
        }
        ImGui::SameLine();

        if (ImGui::Button(Names::Button.c_str()))
        {
            toggle_visible();
        }

        if (_show && ImGui::BeginPopup(Names::Popup.c_str()))
        {
            for (Subject& v : state)
            {
                bool value = v.value;
                if (ImGui::Checkbox(to_string(v.type).c_str(), &value) && value != v.value)
                {
                    v.value = value;
                    v.on_toggle(v.value && _enabled);
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
    template <Type T>
    bool Track<Args...>::enabled() const
    {
        static_assert(equals_any(T, Args...), "Type is not being tracked");

        if (!_enabled)
        {
            return false;
        }

        for (const auto& subject : state)
        {
            if (subject.type == T)
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

    template <Type... Args>
    template <Type T>
    void Track<Args...>::set_enabled(bool value)
    {
        static_assert(equals_any(T, Args...), "Type is not being tracked");

        for (auto& subject : state)
        {
            if (subject.type == T && subject.value != value)
            {
                subject.value = value;
                subject.on_toggle(subject.value);
            }
        }
    }
}
