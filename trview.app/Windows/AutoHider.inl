#pragma once

namespace trview
{
    template <std::ranges::forward_range T, std::ranges::forward_range R, typename F>
    bool AutoHider::apply(T&& full_range, R&& partial_range, Filters& filters)
    {
        return apply(full_range, partial_range, filters.test_and_reset_changed());
    }

    template <std::ranges::forward_range T, std::ranges::forward_range R>
    bool AutoHider::apply(T&& full_range, R&& partial_range, bool filter_changed)
    {
        if (_enabled_changed || (_enabled && filter_changed))
        {
            for (auto& item : full_range)
            {
                if (auto item_ptr = item.lock())
                {
                    bool new_value = !_enabled || std::ranges::find(partial_range, item_ptr) != partial_range.end();
                    item_ptr->set_visible(new_value);
                }
            }
            return true;
        }
        return false;
    }
}
