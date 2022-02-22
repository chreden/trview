#pragma once

namespace trview
{
    template < typename T >
    void imgui_sort(std::vector<T>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks);

    template < typename T >
    void imgui_sort_weak(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks);

    void imgui_scroll_to_item(bool selected, bool& should_scroll_to);
}

#include "trview_imgui.hpp"