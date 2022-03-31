#pragma once

namespace trview
{
    template < typename T >
    void imgui_sort(std::vector<T>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks);

    template < typename T >
    void imgui_sort_weak(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks);

    struct ImGuiScroller
    {
    public:
        ImGuiScroller();
        void scroll_to_item();
        void fix_scroll();
    private:
        ImVec2 _cursor_pos;
    };
}

#include "trview_imgui.hpp"
