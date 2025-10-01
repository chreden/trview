#pragma once

namespace trview
{
    template < typename T >
    void imgui_sort(std::vector<T>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks, bool force_sort = false);

    template < typename T >
    void imgui_sort_weak(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks, bool force_sort = false);

    struct ImGuiHeader
    {
        std::string name;
        float width;
        std::function<void(bool)> set_checked = nullptr;
        bool checked;
    };

    void imgui_header_row(std::vector<ImGuiHeader> headers);

    struct ImGuiScroller
    {
    public:
        ImGuiScroller();
        void scroll_to_item();
        void fix_scroll();
    private:
        ImVec2 _cursor_pos;
    };

    struct ImGuiAnchor
    {
        enum class Anchor
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight
        };

        std::optional<ImVec2> last_position;
        std::optional<ImVec2> last_padding;
        std::optional<ImVec2> last_size;
        std::optional<ImVec2> last_client_size;
        std::optional<ImVec2> in_window_offset;
        bool docked{ false };
        bool reposition{ true };
        Anchor anchor{ Anchor::TopRight };

        void check_resize(ImVec2 intended_client_size);
        void record_position(ImVec2 intended_client_size);
        void record_size();
    };
}

#include "trview_imgui.hpp"
