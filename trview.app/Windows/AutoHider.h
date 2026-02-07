#pragma once

#include <ranges>
#include "../Filters/Filters.h"

namespace trview
{
    class AutoHider final
    {
    public:
        bool changed() const;
        void check_focus();
        void render();

        template <std::ranges::forward_range T, std::ranges::forward_range R, typename F>
        bool apply(T&& full_range, R&& partial_range, Filters& filters);

        template <std::ranges::forward_range T, std::ranges::forward_range R>
        bool apply(T&& full_range, R&& partial_range, bool filter_changed = true);
    private:
        bool _was_focused{ false };
        bool _enabled{ false };
        bool _enabled_changed{ false };
    };
}

#include "AutoHider.inl"
